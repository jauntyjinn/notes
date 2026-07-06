#include <Arduino.h>
#include <ArduinoJson.h>
#include "Ticker.h"
#include <stdio.h>
#include <string.h>
#include "HardwareSerial.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <time.h>  // 添加时间头文件

// 全局对象定义
WiFiClient espClient;
PubSubClient client(espClient);

// 两个独立的串口
HardwareSerial tianwenSerial(1);    // UART1: 天问语音模块
HardwareSerial raspberrySerial(2);  // UART2: 树莓派通信

// DHT11温湿度传感器配置
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi连接参数
const char* ssid     = "redian421-05";
const char* password = "42105redian";

// 华为云设备信息
#define DEVICE_ID        "68df72b50a9ab42ae58083ff_ESP32"
#define DEVICE_SECRET    "dgy197606"
const char* CLIENT_ID     = "68df72b50a9ab42ae58083ff_ESP32_0_0_2025100308";
const char* MQTT_USER     = "68df72b50a9ab42ae58083ff_ESP32";
const char* MQTT_PASSWORD = "59d501779a8f744a5bf84ba88488d62ea9824fb902abb5e9e15149595cbdfe00";
const char* MQTT_SERVER   = "9a3acfcff3.st1.iotda-device.cn-east-3.myhuaweicloud.com";
const int   MQTT_PORT     = 1883;

// MQTT主题定义
#define MQTT_TOPIC_REPORT      "$oc/devices/68df72b50a9ab42ae58083ff_ESP32/sys/properties/report"
#define MQTT_TOPIC_GET         "$oc/devices/68df72b50a9ab42ae58083ff_ESP32/sys/messages/down"
#define MQTT_TOPIC_COMMANDS    "$oc/devices/68df72b50a9ab42ae58083ff_ESP32/sys/commands/"
#define MQTT_TOPIC_CMD_RESPONSE "$oc/devices/68df72b50a9ab42ae58083ff_ESP32/sys/commands/response/request_id="
#define RESPONSE_DATA     "{\"result_code\": 0,\"response_name\": \"COMMAND_RESPONSE\",\"paras\": {\"result\": \"success\"}}"

// 引脚定义
#define RED_LED           16
#define GREEN_LED         18
#define BLUE_LED          17
#define RED_LED2          2
#define GREEN_LED2        4
#define BLUE_LED2         5
#define PHOTORESISTOR_PIN 32
#define CO2_SENSOR_PIN    36

//L298N_1 控制前轮
#define LF_IN1  8
#define LF_IN2  7
#define RF_IN1  3
#define RF_IN2  1
#define ENA1  6
#define ENB1  22

// 时间服务器配置[6](@ref)
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "cn.pool.ntp.org";
const char* ntpServer3 = "time.apple.com";
const long gmtOffset_sec = 8 * 3600; // 东八区（北京时间）UTC+8[6](@ref)
const int daylightOffset_sec = 0;   // 夏令时偏移

// 全局变量
int photoResistorValue;
int co2Value;
float temperature = 0.0;
float humidity = 0.0;
int redValue1 = 0, greenValue1 = 0, blueValue1 = 0;
int redValue2 = 0, greenValue2 = 0, blueValue2 = 0;
bool deviceSwitch1 = false, deviceSwitch2 = false;
String tianwen = "";
String carStatus = "stop";

// 时间相关变量
unsigned long lastTimeSync = 0;
const unsigned long TIME_SYNC_INTERVAL = 3600000; // 1小时同步一次时间
bool timeSynced = false;

// 函数声明
void setupWiFi();
void setupMQTT();
void reconnectMQTT();
void publishSensorData();
void callback(char* topic, byte* payload, unsigned int length);
void sendCommandResponse(char* topic);
void initializePins();
void initializeSensors();
int readCO2FromJW01();
void readDHT11Data();
void setLEDs1(int red, int green, int blue);
void setLEDs2(int red, int green, int blue);
void sendCarCommand(String command);
void processTianwenCommand(String command);
void evaluateEnvironment();

// 时间相关函数声明[6](@ref)
void setupTime();
String getFormattedTime();
String getTimestamp();
bool syncTime();
void printLocalTime();

void setup() {
  analogReadResolution(12);
  
  Serial.begin(115200);
  
  // 初始化两个串口
  tianwenSerial.begin(115200, SERIAL_8N1, 19, 23);    // RX=19, TX=23 用于天问语音
  raspberrySerial.begin(115200, SERIAL_8N1, 16, 17);  // RX=16, TX=17 用于树莓派
  
  Serial.println("====================================");
  Serial.println("ESP32 智慧农业检测小车系统启动");
  Serial.println("串口配置:");
  Serial.println("  UART1 - 天问语音 (GPIO19-RX, GPIO23-TX)");
  Serial.println("  UART2 - 树莓派通信 (GPIO16-RX, GPIO17-TX)");
  Serial.println("====================================");
  
  // 初始化硬件
  initializePins();
  initializeSensors();
  dht.begin();
  
  // 连接WiFi
  setupWiFi();
  
  // 初始化时间服务（在WiFi连接成功后调用）[6](@ref)
  setupTime();
  
  // 初始化MQTT
  setupMQTT();

  Serial.println("系统初始化完成，开始运行...");
}

void loop() {
  // MQTT连接维护
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  
  // 定时处理传感器数据（每5秒）
  long currentTime = millis();
  static long lastReportTime = 0;
  static long lastDHTReadTime = 0;
  
  // 定期时间同步（每小时一次）[6](@ref)
  if (millis() - lastTimeSync > TIME_SYNC_INTERVAL) {
    lastTimeSync = millis();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("执行定期时间同步...");
      syncTime();
      if (timeSynced) {
        printLocalTime();
      }
    }
  }
  
  // 每2秒读取一次DHT11传感器
  if (currentTime - lastDHTReadTime > 2000) {
    lastDHTReadTime = currentTime;
    readDHT11Data();
  }
  
  // 每5秒上报一次数据
  if (currentTime - lastReportTime > 5000) {
    lastReportTime = currentTime;
    co2Value = readCO2FromJW01();
    photoResistorValue = analogRead(PHOTORESISTOR_PIN);
    
    // 环境评估
    evaluateEnvironment();
    
    // 上报数据到华为云
    publishSensorData();
    
    Serial.println("=== 传感器数据更新 ===");
    Serial.printf("温度: %.1f℃ 湿度: %.1f%%\n", temperature, humidity);
    Serial.printf("CO2: %dppm 光照: %d\n", co2Value, photoResistorValue);
  }

  // 处理天问语音指令
  if (tianwenSerial.available() > 0) {
    delay(10);
    tianwen = tianwenSerial.readString();
    tianwen.trim();
    if (tianwen != "" && tianwen.length() > 2) {
      Serial.println("收到语音指令: " + tianwen);
      processTianwenCommand(tianwen);
    }
  }

  // 处理树莓派返回的数据
  if (raspberrySerial.available() > 0) {
    String response = raspberrySerial.readString();
    response.trim();
    if (response != "") {
      Serial.println("树莓派响应: " + response);
    }
  }
}

/**
 * 初始化时间服务[6](@ref)
 */
void setupTime() {
  Serial.println("初始化时间服务...");
  
  // 配置时区[6](@ref)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
  
  // 等待时间同步
  Serial.println("等待时间同步...");
  int retryCount = 0;
  while (!syncTime() && retryCount < 10) {
    Serial.println("时间同步失败，3秒后重试...");
    delay(3000);
    retryCount++;
  }
  
  if (timeSynced) {
    Serial.println("时间同步成功");
    printLocalTime();
  } else {
    Serial.println("时间同步失败，将使用设备运行时间");
  }
}

/**
 * 同步时间[6](@ref)
 */
bool syncTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("获取本地时间失败");
    timeSynced = false;
    return false;
  }
  
  timeSynced = true;
  return true;
}

/**
 * 获取格式化时间字符串 (YYYY-MM-DD HH:MM:SS)[6](@ref)
 */
String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo) || !timeSynced) {
    // 如果时间同步失败，返回设备运行时间
    unsigned long seconds = millis() / 1000;
    unsigned long hours = seconds / 3600;
    unsigned long minutes = (seconds % 3600) / 60;
    unsigned long secs = seconds % 60;
    
    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, secs);
    return "DeviceTime:" + String(timeStr);
  }
  
  char timeString[64];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeString);
}

/**
 * 获取时间戳（毫秒级）[7](@ref)
 */
String getTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  // 如果时间同步成功，返回真实时间戳，否则返回设备运行时间
  if (timeSynced) {
    unsigned long long timestamp = (unsigned long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return String(timestamp);
  } else {
    return String(millis());
  }
}

/**
 * 打印本地时间（用于调试）[6](@ref)
 */
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("获取本地时间失败！");
    return;
  }
  
  Serial.println(&timeinfo, "当前时间: %Y-%m-%d %H:%M:%S");
}

/**
 * 读取DHT11温湿度传感器数据
 */
void readDHT11Data() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("读取DHT11失败！");
    return;
  }
  
  humidity = h;
  temperature = t;
}

/**
 * 环境评估函数
 */
void evaluateEnvironment() {
  // 简单的环境评估逻辑
  bool tempGood = (temperature >= 18 && temperature <= 30);
  bool humidityGood = (humidity >= 40 && humidity <= 70);
  bool co2Good = (co2Value <= 1000);
  bool lightGood = (photoResistorValue > 500);
  
  // 控制指示灯
  if (tempGood && humidityGood && co2Good && lightGood) {
    // 环境适宜 - 绿灯
    setLEDs1(0, 255, 0);
    deviceSwitch1 = true;
    Serial.println("环境评估: 适宜 - 绿灯");
  } else {
    // 环境不适宜 - 红灯
    setLEDs1(255, 0, 0);
    deviceSwitch1 = true;
    Serial.println("环境评估: 不适宜 - 红灯");
  }
  
  // 自动控制补光灯
  if (!lightGood) {
    setLEDs2(255, 255, 255); // 白光补光
    deviceSwitch2 = true;
    Serial.println("光照不足 - 开启补光灯");
  } else {
    setLEDs2(0, 0, 0);
    deviceSwitch2 = false;
  }
}

/**
 * 处理天问语音指令
 */
void processTianwenCommand(String command) {
  command.toLowerCase();
  
  // LED控制指令
  if (command.indexOf("redon") != -1) { 
    redValue1 = 255; 
    setLEDs1(redValue1, greenValue1, blueValue1);
  }
  else if (command.indexOf("greenon") != -1) { 
    greenValue1 = 255; 
    setLEDs1(redValue1, greenValue1, blueValue1);
  }
  else if (command.indexOf("blueon") != -1) { 
    blueValue1 = 255; 
    setLEDs1(redValue1, greenValue1, blueValue1);
  }
  else if (command.indexOf("redoff") != -1) { 
    redValue1 = 0; 
    setLEDs1(redValue1, greenValue1, blueValue1);
  }
  else if (command.indexOf("greenoff") != -1) { 
    greenValue1 = 0; 
    setLEDs1(redValue1, greenValue1, blueValue1);
  }
  else if (command.indexOf("blueoff") != -1) { 
    blueValue1 = 0; 
    setLEDs1(redValue1, greenValue1, blueValue1);
  }
  
  // 第二组LED控制指令
  else if (command.indexOf("red2on") != -1) { 
    redValue2 = 255; 
    setLEDs2(redValue2, greenValue2, blueValue2);
  }
  else if (command.indexOf("green2on") != -1) { 
    greenValue2 = 255; 
    setLEDs2(redValue2, greenValue2, blueValue2);
  }
  else if (command.indexOf("blue2on") != -1) { 
    blueValue2 = 255; 
    setLEDs2(redValue2, greenValue2, blueValue2);
  }
  else if (command.indexOf("red2off") != -1) { 
    redValue2 = 0; 
    setLEDs2(redValue2, greenValue2, blueValue2);
  }
  else if (command.indexOf("green2off") != -1) { 
    greenValue2 = 0; 
    setLEDs2(redValue2, greenValue2, blueValue2);
  }
  else if (command.indexOf("blue2off") != -1) { 
    blueValue2 = 0; 
    setLEDs2(redValue2, greenValue2, blueValue2);
  }
  
  // 小车运动控制指令
  else if (command.indexOf("forward") != -1 || command.indexOf("前进") != -1) { 
    sendCarCommand("forward"); 
  }
  else if (command.indexOf("backward") != -1 || command.indexOf("后退") != -1) { 
    sendCarCommand("backward"); 
  }
  else if (command.indexOf("left") != -1 || command.indexOf("左转") != -1) { 
    sendCarCommand("left"); 
  }
  else if (command.indexOf("right") != -1 || command.indexOf("右转") != -1) { 
    sendCarCommand("right"); 
  }
  else if (command.indexOf("stop") != -1 || command.indexOf("停止") != -1) { 
    sendCarCommand("stop"); 
  }
  
  // 环境控制指令
  else if (command.indexOf("开灯") != -1 || command.indexOf("light on") != -1) {
    setLEDs1(255, 255, 255);
    deviceSwitch1 = true;
  }
  else if (command.indexOf("关灯") != -1 || command.indexOf("light off") != -1) {
    setLEDs1(0, 0, 0);
    deviceSwitch1 = false;
  }
  else if (command.indexOf("补光开") != -1) {
    setLEDs2(255, 255, 255);
    deviceSwitch2 = true;
  }
  else if (command.indexOf("补光关") != -1) {
    setLEDs2(0, 0, 0);
    deviceSwitch2 = false;
  }
  
  Serial.println("语音指令处理完成");
}

/**
 * 发送小车控制命令给树莓派
 */
void sendCarCommand(String command) {
  raspberrySerial.println(command);
  carStatus = command;
  Serial.println("发送小车命令: " + command);
  
  // 更新小车状态到华为云
  char properties[200];
  char jsonBuf[300];
  sprintf(properties, "\"CarStatus\":\"%s\"}}]}", command.c_str());
  sprintf(jsonBuf, "{\"services\":[{\"service_id\":\"Arduino\",\"properties\":{%s", properties);
  
  if (client.publish(MQTT_TOPIC_REPORT, jsonBuf)) {
    Serial.println("上报小车状态成功");
  } else {
    Serial.println("上报小车状态失败");
  }
}

/**
 * 初始化引脚模式
 */
void initializePins() {
  Serial.println("初始化引脚...");
  
  // 初始化第一组LED引脚
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  
  // 初始化第二组LED引脚
  pinMode(RED_LED2, OUTPUT);
  pinMode(GREEN_LED2, OUTPUT);
  pinMode(BLUE_LED2, OUTPUT);
  
  // 初始化传感器引脚
  pinMode(PHOTORESISTOR_PIN, INPUT);
  pinMode(CO2_SENSOR_PIN, INPUT);
  pinMode(DHTPIN, INPUT_PULLUP);
  
  // 初始化两组LED为熄灭状态
  setLEDs1(0, 0, 0);
  setLEDs2(0, 0, 0);
  
  Serial.println("引脚初始化完成");
}

/**
 * 初始化传感器
 */
void initializeSensors() {
  Serial.println("初始化传感器...");
  delay(2000);
  Serial.println("传感器初始化完成");
}

/**
 * 读取JW01 CO2传感器数据
 */
int readCO2FromJW01() {
  int analogValue = analogRead(CO2_SENSOR_PIN);
  
  if (analogValue <= 0 || analogValue >= 4095) {
    Serial.println("CO2传感器读取异常");
    return 0;
  }
  
  float voltage = analogValue * (3.3 / 4095.0);
  
  int co2ppm;
  if (voltage == 0) {
    Serial.println("CO2传感器电压过低");
    co2ppm = 0;
  } else if (voltage < 2.0) {
    co2ppm = (voltage / 2.0) * 2000;
  } else if (voltage <= 3.0) {
    co2ppm = 2000 + ((voltage - 2.0) / 1.0) * 3000;
  } else {
    Serial.println("CO2传感器电压异常");
    co2ppm = 5000;
  }
  
  return co2ppm;
}

/**
 * 连接WiFi网络
 */
void setupWiFi() {
  Serial.printf("连接到WiFi: %s ...\n", ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi连接成功");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi连接失败");
  }
}

/**
 * 初始化MQTT客户端
 */
void setupMQTT() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  Serial.println("MQTT客户端初始化完成");
}

/**
 * 重连MQTT服务器
 */
void reconnectMQTT() {
  Serial.println("尝试连接到MQTT服务器...");
  
  int attempts = 0;
  while (!client.connected() && attempts < 5) {
    if (client.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("MQTT连接成功");
      client.subscribe(MQTT_TOPIC_COMMANDS);
      client.subscribe(MQTT_TOPIC_GET);
      Serial.println("已订阅命令主题");
    } else {
      Serial.printf("MQTT连接失败，错误代码: %d，3秒后重试...\n", client.state());
      delay(3000);
      attempts++;
    }
  }
  
  if (!client.connected()) {
    Serial.println("MQTT连接多次尝试失败");
  }
}

/**
 * 上报传感器数据到华为云平台（已添加时间字段）
 */
void publishSensorData() {
  char properties[600];  // 增加缓冲区大小以容纳时间字段
  char jsonBuf[900];
  
  // 获取当前时间和时间戳[6](@ref)
  String currentTime = getFormattedTime();
  String timestamp = getTimestamp();
  
  // 上报所有传感器数据和状态，包括时间
  sprintf(properties, 
          "\"Time\":\"%s\",\"Timestamp\":%s,\"Temperature\":%.1f,\"Humidity\":%.1f,\"CO2\":%d,\"Photores\":%d,"
          "\"RED1\":%d,\"GREEN1\":%d,\"BLUE1\":%d,\"Switch1\":%d,"
          "\"RED2\":%d,\"GREEN2\":%d,\"BLUE2\":%d,\"Switch2\":%d,"
          "\"CarStatus\":\"%s\"}}]}",
          currentTime.c_str(), timestamp.c_str(),
          temperature, humidity, co2Value, photoResistorValue,
          redValue1, greenValue1, blueValue1, deviceSwitch1,
          redValue2, greenValue2, blueValue2, deviceSwitch2,
          carStatus.c_str());
  
  sprintf(jsonBuf, "{\"services\":[{\"service_id\":\"Arduino\",\"properties\":{%s", properties);
  
  if (client.publish(MQTT_TOPIC_REPORT, jsonBuf)) {
    Serial.println("数据上报华为云成功");
    Serial.println("上报时间: " + currentTime);
  } else {
    Serial.println("数据上报华为云失败");
  }
}

/**
 * MQTT消息回调函数，处理平台下发的命令
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n=== 收到平台下发消息 ===");
  Serial.print("主题: ");
  Serial.println(topic);
  
  payload[length] = '\0';
  String message = String((char*)payload);
  Serial.print("内容: ");
  Serial.println(message);
  
  if (strstr(topic, MQTT_TOPIC_COMMANDS)) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
      Serial.println("JSON解析失败");
      return;
    }
    
    String service_id = doc["service_id"];
    String command_name = doc["command_name"];
    
    Serial.printf("服务ID: %s, 命令: %s\n", service_id.c_str(), command_name.c_str());
    
    // 处理LED控制命令
    if (service_id == "Arduino" || service_id == "Agriculture") {
      if (doc["paras"].containsKey("RED1")) {
        redValue1 = doc["paras"]["RED1"];
        greenValue1 = doc["paras"]["GREEN1"];
        blueValue1 = doc["paras"]["BLUE1"];
        deviceSwitch1 = doc["paras"]["Switch1"];
        
        Serial.printf("LED1控制 - R:%d, G:%d, B:%d, Switch:%d\n", 
                      redValue1, greenValue1, blueValue1, deviceSwitch1);
        
        if (deviceSwitch1) {
          setLEDs1(redValue1, greenValue1, blueValue1);
        } else {
          setLEDs1(0, 0, 0);
        }
      }
      
      if (doc["paras"].containsKey("RED2")) {
        redValue2 = doc["paras"]["RED2"];
        greenValue2 = doc["paras"]["GREEN2"];
        blueValue2 = doc["paras"]["BLUE2"];
        deviceSwitch2 = doc["paras"]["Switch2"];
        
        Serial.printf("LED2控制 - R:%d, G:%d, B:%d, Switch:%d\n", 
                      redValue2, greenValue2, blueValue2, deviceSwitch2);
        
        if (deviceSwitch2) {
          setLEDs2(redValue2, greenValue2, blueValue2);
        } else {
          setLEDs2(0, 0, 0);
        }
      }
    }
    
    // 处理小车运动命令
    if (service_id == "Arduino" || service_id == "Car") {
      String direction = doc["paras"]["direction"];
      Serial.println("收到小车控制命令: " + direction);
      sendCarCommand(direction);
    }
    
    // 发送响应
    char requestId[100] = {0};
    char* pstr = topic;
    char* p = requestId;
    int flag = 0;
    
    while (*pstr) {
      if (flag) *p++ = *pstr;
      if (*pstr == '=') flag = 1;
      pstr++;
    }
    *p = '\0';
    
    Serial.print("Request ID: ");
    Serial.println(requestId);
    
    char responseTopic[200] = {0};
    strcat(responseTopic, MQTT_TOPIC_CMD_RESPONSE);
    strcat(responseTopic, requestId);
    
    sendCommandResponse(responseTopic);
  }
  else if (strstr(topic, MQTT_TOPIC_GET)) {
    Serial.println("收到下行消息，暂不处理");
  }
  
  Serial.println("========================");
}

/**
 * 发送命令响应到平台
 */
void sendCommandResponse(char* topic) {
  if (client.publish(topic, RESPONSE_DATA)) {
    Serial.println("命令响应发送成功");
  } else {
    Serial.println("命令响应发送失败");
  }
}

/**
 * 控制第一组三色LED
 */
void setLEDs1(int red, int green, int blue) {
  analogWrite(RED_LED, 255 - red);
  analogWrite(GREEN_LED, 255 - green);
  analogWrite(BLUE_LED, 255 - blue);
  
  Serial.printf("设置LED1颜色 - R:%d, G:%d, B:%d\n", red, green, blue);
}

/**
 * 控制第二组三色LED
 */
void setLEDs2(int red, int green, int blue) {
  analogWrite(RED_LED2, 255 - red);
  analogWrite(GREEN_LED2, 255 - green);
  analogWrite(BLUE_LED2, 255 - blue);
  
  Serial.printf("设置LED2颜色 - R:%d, G:%d, B:%d\n", red, green, blue);
}