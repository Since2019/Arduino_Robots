#include <Servo.h>

Servo esc;  // 创建一个伺服对象控制电调

void setup() {
  esc.attach(9);  // D9 接电调信号线
  esc.writeMicroseconds(1000);  // 初始最小油门
  delay(2000);  // 给电调时间进行启动音
}

void loop() {
  esc.writeMicroseconds(1200);  // 微小油门（马达可能开始转）
  delay(3000);  // 保持一段时间

  esc.writeMicroseconds(1700);  // 加速
  delay(3000);  // 保持一段时间

  esc.writeMicroseconds(1000);  // 减速
  delay(3000);
}
