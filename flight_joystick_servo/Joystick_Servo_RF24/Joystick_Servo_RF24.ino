#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser2.0.h"

#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>  // 支持软 SPI 模式

// USB Host 对象
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

// NRF24 使用软 SPI：CE, CSN, MOSI, MISO, SCK
RF24 radio(9, 10, 8, 7, 6);
const byte address[6] = "00001";

struct JoystickData {
  uint8_t angleX;
  uint8_t angleY;
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // 初始化 USB Host
  if (Usb.Init() == -1) {
    Serial.println("❌ USB Host 初始化失败");
    while (1);
  }
  Hid.SetReportParser(0, &Joy);
  Serial.println("✅ USB Host 初始化成功");

  // 初始化 NRF24
  if (!radio.begin()) {
    Serial.println("❌ NRF24 初始化失败");
  } else {
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(90);
    radio.setAutoAck(false);
    radio.openWritingPipe(address);
    radio.stopListening();
    Serial.println("✅ NRF24 初始化成功（软SPI）");
  }
}

void loop() {
  Usb.Task();

  int Xval, Yval, Hat, Twist, Slider, Button;
  JoyEvents.GetValues(Xval, Yval, Hat, Twist, Slider, Button);

  int angleX = map(Twist, 0, 255, 180, 0);
  int angleY = (Slider > 10) ? map(Slider, 0, 255, 180, 0) : map(Yval, 0, 1023, 0, 180);

  angleX = constrain(angleX, 0, 180);
  angleY = constrain(angleY, 0, 180);

  JoystickData data = { (uint8_t)angleX, (uint8_t)angleY };
  bool ok = radio.write(&data, sizeof(data));

  Serial.print("🎮 X=");
  Serial.print(angleX);
  Serial.print("°, Y=");
  Serial.print(angleY);
  Serial.println(ok ? " ✅ 发射成功" : " ❌ 发送失败");

  delay(100);
}
