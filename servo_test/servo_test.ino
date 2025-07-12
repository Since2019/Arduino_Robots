#include <Servo.h>

Servo servo;

void setup() {
  servo.attach(5);  // 舵机信号线接 D5
}

void loop() {
  for (int i = 0; i <= 180; i += 10) {
    servo.write(i);
    delay(300);
  }
  for (int i = 180; i >= 0; i -= 10) {
    servo.write(i);
    delay(300);
  }
}
