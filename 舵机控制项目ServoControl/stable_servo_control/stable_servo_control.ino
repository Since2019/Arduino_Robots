
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <Servo.h>
#include <SPI.h>
#include "hidjoystickrptparser.h"

// 舵机对象
Servo servoX;
Servo servoY;

// 防抖 tracker
int lastAngleX = 90;
int lastAngleY = 90;

unsigned long lastUpdateX = 0;
unsigned long lastUpdateY = 0;
const unsigned long LOCKOUT_MS = 0;

// USB Host
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

// 当前值
uint8_t joyX = 127;
uint8_t joyY = 127;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  servoX.attach(5);
  servoY.attach(6);
  servoX.write(90);
  servoY.write(90);

  if (Usb.Init() == -1) {
    Serial.println("USB Host 初始化失败");
    while (1);
  }

  Hid.SetReportParser(0, &Joy);
}

int mapJoystickToAngle(uint8_t val, int center = 127, int deadzone = 12) {
  val = constrain(val, 0, 255);

  // 精准判断是否处于 deadzone，确保只有回中时归中
  if (val >= center - deadzone && val <= center + deadzone)
    return 90;

  // 向左映射
  if (val < center - deadzone)
    return map(val, 0, center - deadzone - 1, 0, 89);

  // 向右映射
  return map(val, center + deadzone + 1, 255, 91, 180);
}

void loop() {
  Usb.Task();

  if (JoyEvents.hasChanged()) {
    joyX = JoyEvents.getX();
    joyY = JoyEvents.getY();

    int angleX = mapJoystickToAngle(255 - joyX); // 反转 X
    int angleY = mapJoystickToAngle(joyY);

    unsigned long now = millis();

//    if ((now - lastUpdateX) > LOCKOUT_MS && angleX != lastAngleX) {
      servoX.write(angleX);
      lastAngleX = angleX;
      lastUpdateX = now;

      Serial.print("[X轴] joyX: ");
      Serial.print(joyX);
      Serial.print(" → 角度: ");
      Serial.println(angleX);
//    }

//    if ((now - lastUpdateY) > LOCKOUT_MS && angleY != lastAngleY) {
      servoY.write(angleY);
      lastAngleY = angleY;
      lastUpdateY = now;

      Serial.print("[Y轴] joyY: ");
      Serial.print(joyY);
      Serial.print(" → 角度: ");
      Serial.println(angleY);
//    }
  }

  delay(10);
}
