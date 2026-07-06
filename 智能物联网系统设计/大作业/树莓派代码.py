import serial
import time
import RPi.GPIO as GPIO
import math

# ======================
# 引脚定义（占位符）
# ======================
# L298N_1 控制前轮
LF_IN1 = 5
LF_IN2 = 6
RF_IN1 = 19
RF_IN2 = 13
ENA1 = 12
ENB1 = 26

# L298N_2 控制后轮
LB_IN1 = 20
LB_IN2 = 16
RB_IN1 = 25
RB_IN2 = 21
ENA2 = 23
ENB2 = 24

# ======================
# 初始化 GPIO
# ======================
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

motor_pins = [LF_IN1, LF_IN2, RF_IN1, RF_IN2,
              LB_IN1, LB_IN2, RB_IN1, RB_IN2,
              ENA1, ENB1, ENA2, ENB2]
for pin in motor_pins:
    GPIO.setup(pin, GPIO.OUT)

# 初始化 PWM
pwm_ENA1 = GPIO.PWM(ENA1, 100)
pwm_ENB1 = GPIO.PWM(ENB1, 100)
pwm_ENA2 = GPIO.PWM(ENA2, 100)
pwm_ENB2 = GPIO.PWM(ENB2, 100)

pwm_ENA1.start(0)
pwm_ENB1.start(0)
pwm_ENA2.start(0)
pwm_ENB2.start(0)


# 串口配置
ser = serial.Serial('/dev/serial0', 115200, timeout=0.5)
time.sleep(1)
print("树莓派串口初始化完成，等待ESP32指令...")

def set_motor(lf, rf, lb, rb):
    """lf, rf, lb, rb = 1正转, -1反转, 0停止"""
    GPIO.output(LF_IN1, lf > 0)
    GPIO.output(LF_IN2, lf <= 0)
    GPIO.output(RF_IN1, rf > 0)
    GPIO.output(RF_IN2, rf <= 0)
    GPIO.output(LB_IN1, lb > 0)
    GPIO.output(LB_IN2, lb <= 0)
    GPIO.output(RB_IN1, rb > 0)
    GPIO.output(RB_IN2, rb <= 0)

    pwm_ENA1.ChangeDutyCycle(abs(lf))
    pwm_ENB1.ChangeDutyCycle(abs(rf))
    pwm_ENA2.ChangeDutyCycle(abs(lb))
    pwm_ENB2.ChangeDutyCycle(abs(rb))

def forward():
    set_motor(50, 50, 50, 50)
def backward():
    set_motor(-50, -50, -50, -50)
def stop():
    set_motor(0, 0, 0, 0)
def left_turn():
    set_motor(-90, 90, -90, 90)
def right_turn():
    set_motor(90, -90, 90, -90)
    

while True:
    if ser.in_waiting > 0:
        cmd = ser.readline().decode('utf-8').strip()
        print(f"接收到指令：{cmd}")

        if cmd == "forward":
            forward()
        elif cmd == "backward":
            backward()
        elif cmd == "left":
            left_turn()
        elif cmd == "right":
            right_turn()
        elif cmd == "stop":
            stop()
        else:
            print("未知指令！")