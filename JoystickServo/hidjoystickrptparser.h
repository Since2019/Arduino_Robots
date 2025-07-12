#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include <usbhid.h>
#include <Servo.h>
extern Servo servoX;
extern Servo servoY;

struct GamePadEventData {
        uint8_t X, Y, Z1, Z2, Rz;
};




class JoystickEvents {

  // 工具函数：将 joystick 值（0–255）映射为舵机角度（0–180）
int mapJoystickToAngle(uint8_t val, int center = 127, int deadzone = 8) {
  if (abs(val - center) < deadzone) {
    return 90; // 保持中间角度
  }

  // 限制映射范围，避免极限偏移出错
  val = constrain(val, 0, 255);
  
  if (val < center - deadzone) {
    return map(val, 0, center - deadzone, 0, 90);
  } else {
    return map(val, center + deadzone, 255, 90, 180);
  }
}

public:
    void OnGamePadChanged(const GamePadEventData *evt) {
        Serial.println("手柄状态已改变！");
        uint8_t x = evt->X;
        uint8_t y = evt->Y;

        int angleX = mapJoystickToAngle(x);
        int angleY = mapJoystickToAngle(y);

        servoX.write(angleX);
        servoY.write(angleY);

        Serial.print("X1: "); Serial.println(x);
        Serial.print("Y1: "); Serial.println(y);
        Serial.print("→ 舵机X: "); Serial.println(angleX);
        Serial.print("→ 舵机Y: "); Serial.println(angleY);
    }

    void OnHatSwitch(uint8_t hat);
    void OnButtonUp(uint8_t but_id);
    void OnButtonDn(uint8_t but_id);
};




#define RPT_GEMEPAD_LEN		5

class JoystickReportParser : public HIDReportParser {
        JoystickEvents *joyEvents;

        uint8_t oldPad[RPT_GEMEPAD_LEN];
        uint8_t oldHat;
        uint16_t oldButtons;

public:
        JoystickReportParser(JoystickEvents *evt);

        virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

#endif // __HIDJOYSTICKRPTPARSER_H__
