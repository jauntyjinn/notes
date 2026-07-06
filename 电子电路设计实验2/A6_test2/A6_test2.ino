#include <LiquidCrystal.h>

// ===================== 引脚定义 =====================
const int LM35_PIN   = A0;
const int LED_PIN    = A1;
const int RELAY_PIN  = A2;
const int ENC_A_PIN  = 8;
const int ENC_B_PIN  = 7;
const int ENC_SW_PIN = 6;

// LCD1602: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ===================== 温度参数 =====================
float setTemp    = 40.0;
float currentTemp = 0.0;
float displayTemp = 0.0;   // 锁存给LCD/串口显示用，与控制逻辑同步

const float TEMP_MIN = 0.0;
const float TEMP_MAX = 100.0;

// 半宽迟滞：setTemp=40.0, CONTROL_BAND=0.5
// 低于 39.5°C 开始加热，高于 40.5°C 停止加热
const float CONTROL_BAND = 0.5;

// 传感器异常保护范围
const float VALID_TEMP_MIN = 2.0;
const float VALID_TEMP_MAX = 100.0;

// ===================== 滑动平均滤波（8点循环缓冲）=====================
// 替代前人方案的固定偏差补偿（temp-1.95），对任意工况下的噪声均有效
#define FILTER_SIZE 8

float tempBuffer[FILTER_SIZE] = {0};
int   bufIndex  = 0;
bool  bufFilled = false;

float readTempFiltered() {
    float raw = analogRead(LM35_PIN) * (5000.0 / 1023.0) / 10.0;

    tempBuffer[bufIndex] = raw;
    bufIndex = (bufIndex + 1) % FILTER_SIZE;
    if (bufIndex == 0) bufFilled = true;

    int count = bufFilled ? FILTER_SIZE : bufIndex;
    float sum = 0.0;
    for (int i = 0; i < count; i++) sum += tempBuffer[i];
    return sum / count;
}

// ===================== 控制状态 =====================
bool heating   = false;
bool manualOff = false;   // 长按编码器按钮切换，强制关闭加热

// 继电器最短保持时间（防止高频切换损坏器件）
const unsigned long MIN_RELAY_HOLD_MS = 5000;
unsigned long lastRelayChangeTime = 0;

// ===================== 趋势判断 =====================
// 比较5秒前后的displayTemp均值，避免相邻采样噪声导致趋势字符抖动
char  trendChar        = '~';
float trendRefTemp     = 0.0;
unsigned long lastTrendCheckTime = 0;
const unsigned long TREND_CHECK_INTERVAL_MS = 5000;
const float         TREND_THRESHOLD         = 0.15;

// 每次displayTemp更新后调用，内部有5秒节流
void updateTrend() {
    unsigned long now = millis();
    if (now - lastTrendCheckTime < TREND_CHECK_INTERVAL_MS) return;

    float delta = displayTemp - trendRefTemp;

    if (delta > TREND_THRESHOLD) {
        trendChar = '^';
    } else if (delta < -TREND_THRESHOLD) {
        trendChar = 'v';
    } else {
        // 变化不明显时，加热中辅助显示为升温
        trendChar = heating ? '^' : '~';
    }

    trendRefTemp      = displayTemp;
    lastTrendCheckTime = now;
}

// ===================== 编码器 =====================
int  lastEncA            = HIGH;
bool buttonPressed        = false;
bool rotatedWhilePressed  = false;
unsigned long buttonDownTime = 0;
const unsigned long LONG_PRESS_MS = 500;

// ===================== 显示 / 串口计时 =====================
unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_INTERVAL_MS = 500;

unsigned long lastSerialTime = 0;
const unsigned long SERIAL_INTERVAL_MS = 1000;

// ===================== 函数声明 =====================
void readEncoder();
void readButton();
void updateControl();
void setHeating(bool on);
void forceHeatingOff();
void updateTrend();
void updateDisplay();
void printSerialDebug();

// ===================== setup =====================
void setup() {
    pinMode(RELAY_PIN,  OUTPUT);
    pinMode(LED_PIN,    OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN,   LOW);

    pinMode(ENC_A_PIN,  INPUT_PULLUP);
    pinMode(ENC_B_PIN,  INPUT_PULLUP);
    pinMode(ENC_SW_PIN, INPUT_PULLUP);

    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("A3 Temp Ctrl");
    lcd.setCursor(0, 1); lcd.print("Starting...");

    Serial.begin(9600);
    Serial.println("A3 Temperature Controller");

    // 预填充滤波缓冲区，避免开机读数异常
    for (int i = 0; i < FILTER_SIZE; i++) {
        currentTemp = readTempFiltered();
        delay(10);
    }

    displayTemp        = currentTemp;
    trendRefTemp       = currentTemp;
    lastTrendCheckTime = millis();
    trendChar          = '~';
    lastEncA           = digitalRead(ENC_A_PIN);

    delay(500);
    lcd.clear();
}

// ===================== loop =====================
void loop() {
    // 1. 采样（每次loop只采样一次，控制逻辑用这次的值）
    currentTemp = readTempFiltered();

    // 2. 输入处理
    readEncoder();
    readButton();

    // 3. 控制逻辑（用currentTemp）
    updateControl();

    unsigned long now = millis();

    // 4. 每500ms刷新显示
    //    先锁存displayTemp，再更新趋势，再刷新LCD
    //    保证LCD显示的温度与触发控制时的温度一致
    if (now - lastDisplayTime >= DISPLAY_INTERVAL_MS) {
        lastDisplayTime = now;
        displayTemp = currentTemp;   // 锁存
        updateTrend();               // 5秒节流，放这里每次判断但不一定更新
        updateDisplay();
    }

    // 5. 每1000ms输出串口调试信息
    if (now - lastSerialTime >= SERIAL_INTERVAL_MS) {
        lastSerialTime = now;
        printSerialDebug();
    }
}

// ===================== 编码器旋转 =====================
void readEncoder() {
    int encA = digitalRead(ENC_A_PIN);

    // 检测A相下降沿
    if (lastEncA == HIGH && encA == LOW) {
        int encB = digitalRead(ENC_B_PIN);
        int dir  = (encB == HIGH) ? 1 : -1;

        // 按下旋转：步进1.0°C（10倍速）；松开旋转：步进0.1°C
        bool btnDown = (digitalRead(ENC_SW_PIN) == LOW);
        float step   = btnDown ? 1.0 : 0.1;

        if (btnDown) rotatedWhilePressed = true;

        setTemp += dir * step;
        setTemp  = constrain(setTemp, TEMP_MIN, TEMP_MAX);
    }

    lastEncA = encA;
}

// ===================== 按键处理 =====================
void readButton() {
    bool btnDown = (digitalRead(ENC_SW_PIN) == LOW);

    if (btnDown && !buttonPressed) {
        buttonPressed       = true;
        buttonDownTime      = millis();
        rotatedWhilePressed = false;
    }

    if (!btnDown && buttonPressed) {
        buttonPressed = false;
        unsigned long held = millis() - buttonDownTime;

        // 长按且未旋转：切换manualOff
        if (!rotatedWhilePressed && held >= LONG_PRESS_MS) {
            manualOff = !manualOff;
            if (manualOff) forceHeatingOff();
        }

        rotatedWhilePressed = false;
    }
}

// ===================== 控制逻辑 =====================
void updateControl() {
    if (manualOff) {
        forceHeatingOff();
        return;
    }

    // 传感器异常保护
    if (currentTemp < VALID_TEMP_MIN || currentTemp > VALID_TEMP_MAX) {
        forceHeatingOff();
        return;
    }

    unsigned long now      = millis();
    bool canSwitch = (now - lastRelayChangeTime >= MIN_RELAY_HOLD_MS);

    if (!heating) {
        // 严格小于下限才开启加热
        if (currentTemp < setTemp - CONTROL_BAND && canSwitch) {
            setHeating(true);
        }
    } else {
        // 严格大于上限才停止加热
        if (currentTemp > setTemp + CONTROL_BAND && canSwitch) {
            setHeating(false);
        }
    }
}

// ===================== 设置加热状态 =====================
void setHeating(bool on) {
    if (heating == on) return;
    heating = on;
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
    digitalWrite(LED_PIN,   on ? HIGH : LOW);
    lastRelayChangeTime = millis();
}

// ===================== 强制关闭加热 =====================
void forceHeatingOff() {
    if (heating) {
        heating = false;
        digitalWrite(RELAY_PIN, LOW);
        digitalWrite(LED_PIN,   LOW);
        lastRelayChangeTime = millis();
    } else {
        digitalWrite(RELAY_PIN, LOW);
        digitalWrite(LED_PIN,   LOW);
    }
}

// ===================== LCD显示 =====================
void updateDisplay() {
    // 第一行：T:39.74^C
    lcd.setCursor(0, 0);
    lcd.print("T:");
    if (displayTemp < 10.0) lcd.print(" ");
    lcd.print(displayTemp, 2);
    lcd.print(trendChar);       // 趋势字符：^ 升温 / v 降温 / ~ 稳定
    lcd.print("C      ");       // 尾部空格清除残留字符

    // 第二行：S:40.0°C HEAT / MANUAL OFF
    lcd.setCursor(0, 1);
    if (manualOff) {
        lcd.print("MANUAL OFF      ");
    } else {
        lcd.print("S:");
        if (setTemp < 10.0) lcd.print(" ");
        lcd.print(setTemp, 1);
        lcd.print((char)223);   // °符号
        lcd.print("C ");
        lcd.print(heating ? "HEAT " : "STOP ");
        lcd.print(" ");         // 清除残留
    }
}

// ===================== 串口调试 =====================
void printSerialDebug() {
    Serial.print("Temp=");
    Serial.print(displayTemp, 2);
    Serial.print("C");
    Serial.print(trendChar);
    Serial.print(" Set=");
    Serial.print(setTemp, 1);
    Serial.print("C");
    Serial.print(" [");
    Serial.print(setTemp - CONTROL_BAND, 1);
    Serial.print("~");
    Serial.print(setTemp + CONTROL_BAND, 1);
    Serial.print("]");
    Serial.print(" Mode=");
    Serial.print(manualOff ? "MANUAL_OFF" : "AUTO");
    Serial.print(" Heat=");
    Serial.print(heating ? "ON" : "OFF");
    Serial.print(" Hold=");
    Serial.print(MIN_RELAY_HOLD_MS / 1000);
    Serial.println("s");
}
