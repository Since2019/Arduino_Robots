#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <Servo.h>
#include <SPI.h>
#include "hidjoystickrptparser.h"

// 舵机对象
Servo servoX;
Servo servoY;

// USB Host
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  servoX.attach(5);  // 水平
  servoY.attach(6);  // 垂直
  servoX.write(90);
  servoY.write(90);

  if (Usb.Init() == -1) {
    Serial.println("USB Host 初始化失败");
    while (1);
  }

  Hid.SetReportParser(0, &Joy);
}

void loop() {
  Usb.Task();  // 所有舵机控制已集成在 OnGamePadChanged() 中
}

int mapJoystickToAngle(uint8_t val, int center = 127, int deadzone = 8) {
  if (abs(val - center) < deadzone) return 90;
  val = constrain(val, 0, 255);
  if (val < center - deadzone)
    return map(val, 0, center - deadzone, 0, 90);
  else
    return map(val, center + deadzone, 255, 90, 180);
}
