#ifndef __HIDJOYSTICKRPTPARSER_H__
#define __HIDJOYSTICKRPTPARSER_H__

#include <usbhid.h>

struct GamePadEventData {
    uint8_t dummy1;
    uint8_t dummy2;
    uint8_t dummy3;
    uint8_t X;
    uint8_t Y;
    uint8_t Z;
};

class JoystickEvents {
    uint8_t joyX = 127;
    uint8_t joyY = 127;
    bool changed = false;

public:
    void OnGamePadChanged(const GamePadEventData *evt) {
        joyX = evt->X;
        joyY = evt->Y;
        changed = true;
    }

    bool hasChanged() {
        bool c = changed;
        changed = false;
        return c;
    }

    uint8_t getX() const { return joyX; }
    uint8_t getY() const { return joyY; }

    void OnHatSwitch(uint8_t hat) {}
    void OnButtonUp(uint8_t but_id) {}
    void OnButtonDn(uint8_t but_id) {}
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
