#include <Servo.h>

Servo motorFL, motorFR, motorBL, motorBR;

bool armed = false;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  // 连接 4 个电调（可改为任意支持 PWM 的引脚）
  motorFL.attach(3);   // 前左
  motorFR.attach(5);   // 前右
  motorBL.attach(6);   // 后左
  motorBR.attach(9);   // 后右

  // 初始化最小油门
  setAllMotors(1000);
  delay(2000);  // 等电调启动音
}

void loop() {
  static String line = "";

  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') {
      // 解析一整行 X/Y/Z/Throttle/Btn
      int X = 0, Y = 0, Z = 0, throttleRaw = 0, btn = 0;

      char buf[50];
      line.toCharArray(buf, sizeof(buf));
      char *p = strtok(buf, "/");
      if (p) X = atoi(p);
      if (p = strtok(NULL, "/")) Y = atoi(p);
      if (p = strtok(NULL, "/")) Z = atoi(p);
      if (p = strtok(NULL, "/")) throttleRaw = atoi(p);
      if (p = strtok(NULL, "/")) btn = atoi(p);
      line = "";  // 清空缓存

      // 解锁 / 上锁
      if (btn == 1 && !armed) {
        armed = true;
        Serial.println("✅ ARMED");
      }
      if (btn == 2 && armed) {
        armed = false;
        Serial.println("❌ DISARMED");
        setAllMotors(1000);
      }

      // 油门映射：Slider 0~250 ➜ 1000~2000
      int throttle = map(throttleRaw, 0, 250, 1000, 2000);
      throttle = constrain(throttle, 1000, 2000);

      // 控制量映射（摇杆中心为 512）
      int pitch = map(Y, 0, 1023, -200, 200);   // 前后
      int roll  = map(X, 0, 1023, -200, 200);   // 左右
      int yaw   = map(Z, 0, 255, -100, 100);    // 偏航

      // 应用混控公式
      if (armed) {
        int mFL = throttle + pitch + roll - yaw;
        int mFR = throttle + pitch - roll + yaw;
        int mBL = throttle - pitch + roll + yaw;
        int mBR = throttle - pitch - roll - yaw;

        mFL = constrain(mFL, 1000, 2000);
        mFR = constrain(mFR, 1000, 2000);
        mBL = constrain(mBL, 1000, 2000);
        mBR = constrain(mBR, 1000, 2000);

        motorFL.writeMicroseconds(mFL);
        motorFR.writeMicroseconds(mFR);
        motorBL.writeMicroseconds(mBL);
        motorBR.writeMicroseconds(mBR);

        Serial.print("MOTORS: "); Serial.print(mFL); Serial.print(", ");
        Serial.print(mFR); Serial.print(", ");
        Serial.print(mBL); Serial.print(", ");
        Serial.println(mBR);
      }

    } else {
      line += c;
    }
  }
}

void setAllMotors(int pwm) {
  motorFL.writeMicroseconds(pwm);
  motorFR.writeMicroseconds(pwm);
  motorBL.writeMicroseconds(pwm);
  motorBR.writeMicroseconds(pwm);
}