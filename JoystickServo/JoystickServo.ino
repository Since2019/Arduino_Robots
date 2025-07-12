#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <Servo.h>
#include <SPI.h>
#include "hidjoystickrptparser.h"

// USB Host
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

// 舵机对象
Servo servoX; // 水平
Servo servoY; // 垂直

// 舵机当前角度
int angleX = 90;
int angleY = 90;



void setup() {
  Serial.begin(115200);
  while (!Serial); // 仅用于 Leonardo 等板子

  Serial.println("初始化中...");
  if (Usb.Init() == -1) {
    Serial.println("USB Host 初始化失败");
    while (1); // 卡住
  }

  Hid.SetReportParser(0, &Joy);

  // 舵机初始化
  servoX.attach(5); // 水平舵机接 D5
  servoY.attach(6); // 垂直舵机接 D6
  servoX.write(angleX);
  servoY.write(angleY);
}


void loop() {
  Usb.Task();

//  // 获取摇杆数据
//  uint8_t x = JoyEvents.getX(); // 横向
//  uint8_t y = JoyEvents.getY(); // 纵向
//
//  int newAngleX = mapJoystickToAngle(x);
//  int newAngleY = mapJoystickToAngle(y);
//
//  angleX = newAngleX;
//  servoX.write(angleX);
//  Serial.print("水平: ");
//  Serial.println(angleX);
//
//  angleY = newAngleY;
//  servoY.write(angleY);
//  Serial.print("垂直: ");
//  Serial.println(angleY);

  
//  // 只有当值变化时再写入舵机（避免抖动）
//  if (abs(newAngleX - angleX) > 2) {
//    angleX = newAngleX;
//    servoX.write(angleX);
//    Serial.print("水平: ");
//    Serial.println(angleX);
//  }
//
//  if (abs(newAngleY - angleY) > 2) {
//    angleY = newAngleY;
//    servoY.write(angleY);
//    Serial.print("垂直: ");
//    Serial.println(angleY);
//  }

  delay(50); // 调整响应速度
}
