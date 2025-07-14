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
const unsigned long LOCKOUT_MS = 200;
const int ANGLE_THRESHOLD = 5;  // 小于这个角度变化就不写入

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

  // 如果接近中心区域，才归中
  if (val >= center - deadzone && val <= center + deadzone)
    return 90;

  // 向左移动
  if (val < center - deadzone)
    return map(val, 0, center - deadzone - 1, 0, 89);

  // 向右移动
  if (val > center + deadzone)
    return map(val, center + deadzone + 1, 255, 91, 180);

  return 90; // 兜底，实际上永远不会走到
}
void loop() {
  Usb.Task();

  if (JoyEvents.hasChanged()) {
    joyX = JoyEvents.getX();
    joyY = JoyEvents.getY();

    int angleX = mapJoystickToAngle(255 - joyX); // 反转 X
    int angleY = mapJoystickToAngle(joyY);

    unsigned long now = millis();

    // X轴更新逻辑
    if ((now - lastUpdateX) > LOCKOUT_MS) {
      if (angleX != lastAngleX) {
        servoX.write(angleX);
        lastAngleX = angleX;
        lastUpdateX = now;
      }
    }

    // Y轴更新逻辑
    if ((now - lastUpdateY) > LOCKOUT_MS) {
      if (angleY != lastAngleY) {
        servoY.write(angleY);
        lastAngleY = angleY;
        lastUpdateY = now;
      }
    }
  }

  delay(10);
}
