#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 17
#define CSN_PIN 16

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!radio.begin()) {
    Serial.println("NRF24L01 初始化失败！");
    while(1);
  } else if (!radio.isChipConnected()) {
    Serial.println("⚠️ 无法检测到 NRF 芯片！");
    while(1);
  } else {
    Serial.println("NRF 初始化成功 ✅");
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("开始监听无线数据...");
}

void loop() {
  if (radio.available()) {
    char buffer[32] = {0};
    radio.read(&buffer, sizeof(buffer));
    Serial.print("接收到数据：");
    Serial.println(buffer);
  }
}
