#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <SPI.h>
#include "hidjoystickrptparser.h"

// USB Host 对象
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

// 当前值
uint8_t joyX = 127;
uint8_t joyY = 127;
uint8_t joyZ = 127;

void setup() {
  Serial.begin(115200);    // USB调试口
  Serial1.begin(9600);     // HC-12 通信口

  while (!Serial);         // 等待串口打开

  Serial.println("初始化 USB Host 和 HC-12...");

  if (Usb.Init() == -1) {
    Serial.println("❌ USB Host 初始化失败");
    while (1); // 停住
  } else {
    Serial.println("✅ USB Host 初始化成功");
  }

  Hid.SetReportParser(0, &Joy);
  Serial.println("等待摇杆输入...");
}

void loop() {
  Usb.Task();  // 持续轮询 USB

  if (JoyEvents.hasChanged()) {
    // 从结构体中取出值
    joyX = JoyEvents.getX();
    joyY = JoyEvents.getY();

    // 手动访问 Z 轴值（由于结构中没封装 getter，可直接访问）
    const GamePadEventData* evt = (const GamePadEventData*)Hid.GetReportBuffer();
    if (evt != nullptr) {
      joyZ = evt->Z;
    }

    // 打印调试输出
    Serial.print("X:"); Serial.print(joyX);
    Serial.print(" Y:"); Serial.print(joyY);
    Serial.print(" Z:"); Serial.println(joyZ);

    // 构造数据包
    String payload = "X:" + String(joyX) + ",Y:" + String(joyY) + ",Z:" + String(joyZ);

    // 发送到 HC-12 并检查是否成功
    size_t sent = Serial1.println(payload);
    if (sent > 0) {
      Serial.println("📤 HC-12 发送成功: " + payload);
    } else {
      Serial.println("❌ HC-12 发送失败");
    }
  }

  delay(10);
}