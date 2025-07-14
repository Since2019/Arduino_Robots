/* 
 *  Simplified Logitech Extreme 3D Pro Joystick Report Parser, ready to go.
*/

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser2.0.h"
#include <SPI.h>
#include <Servo.h>

USB                                             Usb;
USBHub                                          Hub(&Usb);
HIDUniversal                                    Hid(&Usb);
JoystickEvents                                  JoyEvents;
JoystickReportParser                            Joy(&JoyEvents);

Servo servoX;  // Z轴控制，控制X轴舵机（已反转）
Servo servoY;  // Y轴控制，控制Y轴舵机

int angleX = 90;  // 初始角度
int angleY = 90;

void setup()
{
  Serial.begin(115200);
  
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

  Serial.println("Start");
  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");
  delay(200);

  if (!Hid.SetReportParser(0, &Joy))
    ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);

  // 初始化舵机
  servoX.attach(5);   // 舵机X接D9
  servoY.attach(6);  // 舵机Y接D10
  servoX.write(angleX);
  servoY.write(angleY);
}

void loop()
{
  int Xval;   // 0 - 1023
  int Yval;   // 0 - 1023
  int Hat;    // 0 - 15;
  int Twist;  // 0 - 255
  int Slider; // 0 - 255
  int Button; // 0 - 12 (0 = No button)
  
  Usb.Task();                                                    // Use to read joystick input to controller
  JoyEvents.PrintValues();                                       // Returns joystick values to user
  JoyEvents.GetValues(Xval, Yval, Hat, Twist, Slider, Button);   // Copies joystick values to user

  // Z轴反转后映射到 angleX（Z轴旋转 → 舵机X）
  angleX = map(Twist, 0, 255, 180, 0);

  // Slider 优先控制舵机Y轴，只有当Slider在“非最下”时使用它
  if (Slider > 10) {
    angleY = map(Slider, 0, 255, 180, 0);  // 反转方向
  } else {
    angleY = map(Yval, 0, 1023, 0, 180);
  }

  // 限制角度范围
  angleX = constrain(angleX, 0, 180);
  angleY = constrain(angleY, 0, 180);

  // 写入舵机角度
  servoX.write(angleX);
  servoY.write(angleY);

  delay(30);  // 控制刷新频率
}
