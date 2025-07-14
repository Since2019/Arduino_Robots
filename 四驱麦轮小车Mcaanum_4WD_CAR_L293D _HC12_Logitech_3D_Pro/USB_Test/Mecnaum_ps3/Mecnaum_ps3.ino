#include <PS3USB.h>
#include <usbhub.h>

// 定义所有方向常量
#define FORWARD 1
#define BACKWARD 2
#define LEFT 3
#define RIGHT 4
#define FORWARD_LEFT 5
#define FORWARD_RIGHT 6
#define BACKWARD_LEFT 7
#define BACKWARD_RIGHT 8
#define TURN_LEFT 9
#define TURN_RIGHT 10
#define STOP 0

// 电机索引
#define BACK_RIGHT_MOTOR 0
#define BACK_LEFT_MOTOR 1
#define FRONT_RIGHT_MOTOR 2
#define FRONT_LEFT_MOTOR 3

struct MOTOR_PINS {
  int pinIN1;
  int pinIN2;
  int pinEn;  // 使能引脚 (PWM)
};

// 更新为Arduino Uno引脚配置 - 根据您的实际接线修改
MOTOR_PINS motorPins[4] = {
  {2, 3, 9},   // BACK_RIGHT_MOTOR (IN1, IN2, EN)
  {4, 5, 10},  // BACK_LEFT_MOTOR
  {6, 7, 11},  // FRONT_RIGHT_MOTOR
  {8, 12, 13}  // FRONT_LEFT_MOTOR
};

#define MAX_MOTOR_SPEED 200  // 最大PWM速度 (0-255)

USB Usb;
USBHub Hub(&Usb);
PS3USB PS3(&Usb);

bool connected = false;
unsigned long lastRecvTime = 0;
const uint16_t SIGNAL_TIMEOUT = 1000;  // 1秒超时

void rotateMotor(int motorNumber, int motorSpeed) {
  if (motorSpeed < 0) {  // 反转
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);
  } 
  else if (motorSpeed > 0) {  // 正转
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  } 
  else {  // 停止
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  }
  
  analogWrite(motorPins[motorNumber].pinEn, abs(motorSpeed));
}

void processCarMovement(int inputValue) {
  switch(inputValue) {
    case FORWARD:
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);                  
      break;
  
    case BACKWARD:
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);   
      break;
  
    case LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);   
      break;
  
    case RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);  
      break;
  
    case FORWARD_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);  
      break;
  
    case FORWARD_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, STOP);  
      break;
  
    case BACKWARD_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, STOP);   
      break;
  
    case BACKWARD_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);   
      break;
  
    case TURN_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);  
      break;
  
    case TURN_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);   
      break;
  
    case STOP:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  
    default:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  }
}

void setup() {
  // 初始化所有电机引脚
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);
    pinMode(motorPins[i].pinEn, OUTPUT);
    rotateMotor(i, STOP);  // 初始时停止所有电机
  }
  
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.println("USB主机控制器初始化失败");
    while(1); // 停止执行
  }
  Serial.println("等待PS3手柄连接...");
}

void loop() {
  Usb.Task();  // 必须定期调用以处理USB事件
  
  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    if (!connected) {
      Serial.println("PS3手柄已连接");
      connected = true;
    }
    lastRecvTime = millis();
    
    // 获取并转换摇杆值 (0-255 -> -127-127)
    int rStickX = map(PS3.getAnalogHat(RightHatX), 0, 255, -127, 127);
    int rStickY = map(PS3.getAnalogHat(RightHatY), 0, 255, -127, 127);
    int lStickX = map(PS3.getAnalogHat(LeftHatX), 0, 255, -127, 127);
    
    // 打印摇杆值（调试用）
    Serial.print("摇杆值: ");
    Serial.print("RX="); Serial.print(rStickX);
    Serial.print(" RY="); Serial.print(rStickY);
    Serial.print(" LX="); Serial.println(lStickX);
    
    // 处理摇杆方向
    if (rStickX < -75 && rStickY > 75) {
      processCarMovement(FORWARD_LEFT);    
    }
    else if (rStickX > 75 && rStickY > 75) {
      processCarMovement(FORWARD_RIGHT);    
    } 
    else if (rStickX < -75 && rStickY < -75) {
      processCarMovement(BACKWARD_LEFT);    
    }
    else if (rStickX > 75 && rStickY < -75) {
      processCarMovement(BACKWARD_RIGHT);    
    }  
    else if (lStickX > 75) {
      processCarMovement(TURN_RIGHT);
    }
    else if (lStickX < -75) {
      processCarMovement(TURN_LEFT);
    }
    else if (rStickY > 75) {
      processCarMovement(FORWARD);  
    }
    else if (rStickY < -75) {
      processCarMovement(BACKWARD);     
    }
    else if (rStickX > 75) {
      processCarMovement(RIGHT);   
    }
    else if (rStickX < -75) {
      processCarMovement(LEFT);    
    } 
    else {
      processCarMovement(STOP);     
    }
  } 
  else {
    if (connected) {
      Serial.println("PS3手柄断开");
      connected = false;
      processCarMovement(STOP);
    }
    
    // 超时处理
    if (millis() - lastRecvTime > SIGNAL_TIMEOUT) {
      processCarMovement(STOP);
    }
  }
  
  // 添加短暂延迟以减少CPU负载
  delay(10);
}
