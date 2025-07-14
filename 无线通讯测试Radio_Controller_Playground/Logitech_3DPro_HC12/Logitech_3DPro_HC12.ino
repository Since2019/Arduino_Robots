/*
 *  Modified version for Arduino Mega + HC-12:
 *  - Reads Logitech Extreme 3D Pro via USB Host Shield
 *  - Sends joystick data over Serial1 (TX1 ➝ HC-12 RXD)
 */

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser2.0.h"
#include <SPI.h>

USB                 Usb;
USBHub              Hub(&Usb);
HIDUniversal        Hid(&Usb);
JoystickEvents      JoyEvents;
JoystickReportParser Joy(&JoyEvents);

void setup() {
  Serial.begin(115200);     // 调试信息输出（USB 串口）
  Serial1.begin(9600);      // HC-12 串口（TX1=18 发，RX1=19 收）

  while (!Serial);          // 等待串口准备好

  Serial.println("Starting USB Host...");
  if (Usb.Init() == -1) {
    Serial.println("USB Host init failed");
  }
  delay(200);

  if (!Hid.SetReportParser(0, &Joy)) {
    Serial.println("Failed to set joystick parser");
  }
}

void loop() {
  int Xval, Yval, Hat, Twist, Slider, Button;

  Usb.Task();
  JoyEvents.GetValues(Xval, Yval, Hat, Twist, Slider, Button);

  // PPM风格串口数据：X/Y/Z/Throttle/Btn\n
  Serial1.print(Xval); Serial1.print('/');
  Serial1.print(Yval); Serial1.print('/');
  Serial1.print(Twist); Serial1.print('/');
  Serial1.print(Slider); Serial1.print('/');
  Serial1.print(Button); Serial1.print('\n');

  // 可选：串口监视器输出
  Serial.print("SEND: ");
  Serial.print(Xval); Serial.print('/');
  Serial.print(Yval); Serial.print('/');
  Serial.print(Twist); Serial.print('/');
  Serial.print(Slider); Serial.print('/');
  Serial.println(Button);

  delay(30);
}