#include <PS3BT.h>
#include <usbhub.h>
#include <Servo.h>

USB Usb;
BTD Btd(&Usb);            // 用于蓝牙手柄（如果是USB有线版请改用 PS3USB）
PS3BT PS3(&Btd);          // 初始化 PS3 手柄控制器

Servo servoX;             // 控制云台X轴
Servo servoY;             // 控制云台Y轴

int angleX = 90;
int angleY = 90;

void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.println("USB Host 初始化失败");
    while (1); // 停止执行
  }
  Serial.println("正在等待PS3连接...");

  servoX.attach(9);  // 舵机X接在D9
  servoY.attach(10); // 舵机Y接在D10
  servoX.write(angleX);
  servoY.write(angleY);
}

void loop() {
  Usb.Task();

  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    int lx = PS3.getAnalogHat(LeftHatX);   // 读取左摇杆X
    int ly = PS3.getAnalogHat(LeftHatY);   // 读取左摇杆Y

    // 中心值约为 128，我们取偏移范围 -128 ~ 127
    int dx = map(lx, 0, 255, -10, 10);
    int dy = map(ly, 0, 255, -10, 10);

    angleX = constrain(angleX + dx, 0, 180);
    angleY = constrain(angleY + dy, 0, 180);

    servoX.write(angleX);
    servoY.write(angleY);

    Serial.print("X: "); Serial.print(angleX);
    Serial.print(" | Y: "); Serial.println(angleY);

    delay(100);
  }
}
