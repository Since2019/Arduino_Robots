#include <AFMotor.h>
#include <SoftwareSerial.h>

// 用于接收来自 HC12 的数据
SoftwareSerial HC12(10, 9); // RX, TX

// 初始化电机控制引脚
AF_DCMotor motor1(1);  // 前左 Front Left
AF_DCMotor motor2(2);  // 前右 Front Right
AF_DCMotor motor3(3);  // 后左 Back Left
AF_DCMotor motor4(4);  // 后右 Back Right

char command = 'S';  // 默认命令为停止

int baseSpeed = 220;  // 普通动作速度
int turnSpeed = 120;  // 原地旋转速度

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);  // 设置 HC12 的波特率

  motor1.setSpeed(baseSpeed);
  motor2.setSpeed(baseSpeed);
  motor3.setSpeed(baseSpeed);
  motor4.setSpeed(baseSpeed);

  Serial.println("Motor control initialized");
  Stop();  // 启动默认停止
}

void loop() {
  if (HC12.available() > 0) {
    char incomingChar = HC12.read();

    if (incomingChar != '\n') {
      command = incomingChar;
      processCommand(command);

      Serial.print("Completed command: ");
      Serial.println(command);
    }
  }
}

void processCommand(char cmd) {
  if (cmd == 'S') {
    Stop();
  } else if (cmd == 'F') {
    forward();
  } else if (cmd == 'B') {
    back();
  } else if (cmd == '1') {
    rotateLeft();
  } else if (cmd == '2') {
    rotateRight();
  } else if (cmd == 'Q') {
    forwardLeft();
  } else if (cmd == 'E') {
    forwardRight();
  } else if (cmd == 'Z') {
    backwardLeft();
  } else if (cmd == 'C') {
    backwardRight();
  } else if (cmd == 'L') {
    left();
  } else if (cmd == 'R') {
    right();
  } else {
    Stop();
  }
}

// 方向控制函数

void forward() {
  setAllSpeed(baseSpeed);
  motor1.run(FORWARD); 
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void back() {
  setAllSpeed(baseSpeed);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void rotateLeft() {
  setAllSpeed(turnSpeed);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}

void rotateRight() {
  setAllSpeed(turnSpeed);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
}

void forwardLeft() {
  setAllSpeed(baseSpeed);
  motor1.run(FORWARD);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(FORWARD);
}

void forwardRight() {
  setAllSpeed(baseSpeed);
  motor1.run(RELEASE);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(RELEASE);
}

void backwardLeft() {
  setAllSpeed(baseSpeed);
  motor1.run(RELEASE);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(RELEASE);
}

void backwardRight() {
  setAllSpeed(baseSpeed);
  motor1.run(BACKWARD);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(BACKWARD);
}

void left() {
  setAllSpeed(baseSpeed);
  motor1.run(FORWARD);   // 前左
  motor2.run(BACKWARD);  // 前右
  motor3.run(BACKWARD);  // 后左
  motor4.run(FORWARD);   // 后右
}

void right() {
  setAllSpeed(baseSpeed);
  motor1.run(BACKWARD);  // 前左
  motor2.run(FORWARD);   // 前右
  motor3.run(FORWARD);   // 后左
  motor4.run(BACKWARD);  // 后右
}

void Stop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

// 辅助函数：统一设置四电机速度
void setAllSpeed(int speed) {
  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(speed);
}