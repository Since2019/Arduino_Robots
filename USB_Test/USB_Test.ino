#include <Usb.h>

USB Usb;

void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.println("USB 初始化失败");
    while (1);  // 死循环防止继续运行
  }
  Serial.println("等待手柄插入...");
}

void loop() {
  Usb.Task();

  // 遍历已连接设备
  for (uint8_t i = 0; i < USB_NUMDEVICES; i++) {
    UsbDevice *dev = Usb.getDevice(i);
    if (dev != NULL && dev->epinfo != NULL) {
      uint8_t buffer[64];
      uint16_t rcvd = 0;
      uint8_t rcode = Usb.inTransfer(dev->epinfo[0].deviceAddress, 1, &rcvd, buffer);
      if (rcode == 0 && rcvd > 0) {
        Serial.print("Raw HID 报文: ");
        for (uint8_t j = 0; j < rcvd; j++) {
          Serial.print(buffer[j], HEX);
          Serial.print(" ");
        }
        Serial.println();
        delay(100);
      }
    }
  }
}
