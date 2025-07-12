#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <Servo.h>
#include <SPI.h>
#include "hidjoystickrptparser.h"

Servo servoX, servoY;
int currentAngleX = 90;
int currentAngleY = 90;

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

unsigned long lastMoveTime = 0;
const int speedDegPerSec = 200; // 匀速每秒60°
const unsigned long frameInterval = 10; // 每帧10ms
const float degPerFrame = speedDegPerSec * (frameInterval / 1000.0);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  servoX.attach(5);
  servoY.attach(6);
  servoX.write(currentAngleX);
  servoY.write(currentAngleY);

  if (Usb.Init() == -1) {
    Serial.println("USB 初始化失败");
    while (1);
  }

  Hid.SetReportParser(0, &Joy);
}

int mapJoystickToAngle(uint8_t val, int center = 127, int deadzone = 5) {
  val = constrain(val, 0, 255);
  if (val >= center - deadzone && val <= center + deadzone) return 90;
  if (val < center - deadzone)
    return map(val, 0, center - deadzone - 1, 0, 89);
  return map(val, center + deadzone + 1, 255, 91, 180);
}



void loop() {
  Usb.Task();

  uint16_t btns = JoyEvents.getButtons();
  bool buttonActive = (btns & 0x0F) != 0; //按钮是否按下
  
  uint8_t joyX = JoyEvents.getX();
  uint8_t joyY = JoyEvents.getY();

  // 按钮控制（匀速）
  if (btns & (1 << 3)) {
    Serial.println("按钮X被按下 → X轴向左"); //(要反转X轴的左右)
    currentAngleX += degPerFrame;
  }
  if (btns & (1 << 1)) {
    Serial.println("按钮B被按下 → X轴向右");
    currentAngleX -= degPerFrame;
  }
  if (btns & (1 << 0)) {
    Serial.println("按钮Y被按下 → Y轴向上");
    currentAngleY -= degPerFrame;
  }
  if (btns & (1 << 2)) {
    Serial.println("按钮A被按下 → Y轴向下");
    currentAngleY += degPerFrame;
  }

  currentAngleX = constrain(currentAngleX, 0, 180);
  currentAngleY = constrain(currentAngleY, 0, 180);
  servoX.write(currentAngleX);
  servoY.write(currentAngleY);



  // 优先按钮控制，只有在按钮不活动时才响应左摇杆
  if (!buttonActive && JoyEvents.hasChanged()) {

    currentAngleX = mapJoystickToAngle(255 - joyX);
    currentAngleY = mapJoystickToAngle(joyY);
    servoX.write(currentAngleX);
    servoY.write(currentAngleY);

    Serial.print("左摇杆控制 -> X:");
    Serial.print(currentAngleX);
    Serial.print(" Y:");
    Serial.println(currentAngleY);
  }

  delay(frameInterval);
}

//void loop() {
//  Usb.Task();
//
//  uint16_t btns = JoyEvents.getButtons();
//  uint8_t joyX = JoyEvents.getX();
//  uint8_t joyY = JoyEvents.getY();
//  
//  if (JoyEvents.hasChanged()) {
//    bool buttonActive = btns & 0x0F; // 只看前四位
//
//    if (!buttonActive && JoyEvents.hasChanged()) {
//        // 左摇杆控制逻辑
//        currentAngleX = mapJoystickToAngle(255 - joyX);
//        currentAngleY = mapJoystickToAngle(joyY);
//    }
//    
//    servoX.write(currentAngleX);
//    servoY.write(currentAngleY);
//    
//// // 打印数据，调试用
////    Serial.print("左摇杆控制 -> X:");
////    Serial.print(currentAngleX);
////    Serial.print(" Y:");
////    Serial.println(currentAngleY);
//  }
//
//    // 匀速移动逻辑（按钮控制）
//    if (btns) {
//    Serial.print("检测到按钮：");
//    Serial.println(btns, BIN);
//  }
//  
//  // 匀速按钮控制映射（已确认正确）
//  if (btns & (1 << 3)) {  // X -> 控制 X轴向左（镜像）
//    Serial.println("按钮X被按下 → X轴向左");
//    currentAngleX += degPerFrame;
//  }
//  if (btns & (1 << 1)) {  // B -> 控制 X轴向右（相反）
//    Serial.println("按钮B被按下 → X轴向右");
//    currentAngleX -= degPerFrame;
//  }
//  if (btns & (1 << 0)) {  // Y -> 控制 Y轴向上
//    Serial.println("按钮Y被按下 → Y轴向上");
//    currentAngleY -= degPerFrame;
//  }
//  
//  if (btns & (1 << 2)) {  // A -> 控制 Y轴向下
//    Serial.println("按钮A被按下 → Y轴向下");
//    currentAngleY += degPerFrame;
//  }
//
//  
//
//  currentAngleX = constrain(currentAngleX, 0, 180);
//  currentAngleY = constrain(currentAngleY, 0, 180);
//  servoX.write(currentAngleX);
//  servoY.write(currentAngleY);
//
//  delay(frameInterval);
//}
