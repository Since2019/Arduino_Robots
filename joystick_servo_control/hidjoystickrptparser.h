#ifndef __HIDJOYSTICKRPTPARSER_H__
#define __HIDJOYSTICKRPTPARSER_H__

#include <usbhid.h>
#include <Servo.h>

extern Servo servoX;
extern Servo servoY;
int mapJoystickToAngle(uint8_t val, int center = 127, int deadzone = 8);

struct GamePadEventData {
    uint8_t dummy1;
    uint8_t dummy2;
    uint8_t dummy3;
    uint8_t X;
    uint8_t Y;
    uint8_t Z;
};
class JoystickEvents {
public:
    void OnGamePadChanged(const GamePadEventData *evt) {
//        Serial.println("手柄状态已改变！");
        uint8_t x = 255 - evt->X;  // 左右反转
        uint8_t y = evt->Y;

        int angleX = mapJoystickToAngle(x);
        int angleY = mapJoystickToAngle(y);

        servoX.write(angleX);
        servoY.write(angleY);
//
//        Serial.print("X1: "); Serial.println(x);
//        Serial.print("Y1: "); Serial.println(y);
//        Serial.print("→ 舵机X: "); Serial.println(angleX);
//        Serial.print("→ 舵机Y: "); Serial.println(angleY);
    }

    void OnHatSwitch(uint8_t hat);
    void OnButtonUp(uint8_t but_id);
    void OnButtonDn(uint8_t but_id);
};

#define RPT_GEMEPAD_LEN 5

class JoystickReportParser : public HIDReportParser {
    JoystickEvents *joyEvents;
    uint8_t oldPad[RPT_GEMEPAD_LEN];
    uint8_t oldHat;
    uint16_t oldButtons;

public:
    JoystickReportParser(JoystickEvents *evt);
    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

#endif
