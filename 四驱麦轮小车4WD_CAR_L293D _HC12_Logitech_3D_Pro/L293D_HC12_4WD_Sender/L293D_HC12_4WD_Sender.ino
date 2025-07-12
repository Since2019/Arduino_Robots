#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser2.0.h"
#include <SPI.h>

USB USB;
USBHub Hub(&USB);
HIDUniversal Hid(&USB);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

// 使用 Serial1 连接 HC-12（Mega 专属）
#define HC12 Serial1

// 死区设置
const int DEADZONE = 125;
const int Z_DEADZONE = 85;

void setup() {
  Serial.begin(115200);   // USB 调试串口
  HC12.begin(9600);       // HC-12 波特率

  while (!Serial);
  Serial.println("Starting USB Host...");

  if (USB.Init() == -1) {
    Serial.println("USB Host init failed");
  }

  delay(200);

  if (!Hid.SetReportParser(0, &Joy)) {
    Serial.println("Failed to set joystick parser");
  }

  Serial.println("Joystick ready.");
}

void loop() {
  int Xval, Yval, Hat, Twist, Slider, Button;

  USB.Task();
  JoyEvents.GetValues(Xval, Yval, Hat, Twist, Slider, Button);

  String command = "S";  // 默认停止

  // ---- Z轴优先判断（原地旋转）----
  if (Twist < (128 - Z_DEADZONE)) {
    command = "1"; // 逆时针
  } else if (Twist > (128 + Z_DEADZONE)) {
    command = "2"; // 顺时针
  }
  // ---- 左前、右前、左后、右后 ----
  else if (Yval > (512 + DEADZONE) && Xval < (512 - DEADZONE)) {
    command = "Q"; // 左前
  } else if (Yval > (512 + DEADZONE) && Xval > (512 + DEADZONE)) {
    command = "E"; // 右前
  } else if (Yval < (512 - DEADZONE) && Xval < (512 - DEADZONE)) {
    command = "Z"; // 左后
  } else if (Yval < (512 - DEADZONE) && Xval > (512 + DEADZONE)) {
    command = "C"; // 右后
  }
  // ---- 单方向 ----
  else if (Yval > (512 + DEADZONE)) {
    command = "F"; // 前进
  } else if (Yval < (512 - DEADZONE)) {
    command = "B"; // 后退
  } else if (Xval < (512 - DEADZONE)) {
    command = "L"; // 左转（原地）
  } else if (Xval > (512 + DEADZONE)) {
    command = "R"; // 右转（原地）
  }

  // 发送命令（带换行）
  HC12.print(command);
  Serial.println("Send: " + command);

  delay(30);
}