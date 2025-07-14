
#ifndef __HIDJOYSTICKRPTPARSER_H__
#define __HIDJOYSTICKRPTPARSER_H__

#include <usbhid.h>

struct GamePadEventData {
    uint8_t reportId;
    uint8_t dummy2;
    uint8_t dummy3;
    uint8_t X;
    uint8_t Y;
    uint8_t Z;
};

class JoystickEvents {
    bool changed = false;
    uint8_t joyX = 127;
    uint8_t joyY = 127;

public:
    void OnGamePadChanged(const GamePadEventData *evt) {
//      分析数据包得到的
        if (evt->reportId == 0x01) {
            joyX = evt->X;
            joyY = evt->Y;
            changed = true;
        }
    }

    void OnHatSwitch(uint8_t hat) {}
    void OnButtonUp(uint8_t but_id) {}
    void OnButtonDn(uint8_t but_id) {}

    uint8_t getX() const { return joyX; }
    uint8_t getY() const { return joyY; }

    bool hasChanged() {
        bool temp = changed;
        changed = false;
        return temp;
    }
};

#define RPT_GEMEPAD_LEN 7

class JoystickReportParser : public HIDReportParser {
    JoystickEvents *joyEvents;
    uint8_t oldPad[RPT_GEMEPAD_LEN];
    uint8_t oldHat;
    uint16_t oldButtons;

public:
    JoystickReportParser(JoystickEvents *evt) : joyEvents(evt), oldHat(0xDE), oldButtons(0) {
        memset(oldPad, 0xD, sizeof(oldPad));
    }

    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
        bool match = true;
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) {
            if (buf[i] != oldPad[i]) {
                match = false;
                break;
            }
        }
        if (!match && joyEvents) {
            joyEvents->OnGamePadChanged((const GamePadEventData*)buf);
            memcpy(oldPad, buf, RPT_GEMEPAD_LEN);
        }

        uint8_t hat = (buf[5] & 0xF);
        if (hat != oldHat && joyEvents) {
            joyEvents->OnHatSwitch(hat);
            oldHat = hat;
        }

        uint16_t buttons = (0x0000 | buf[6]);
        buttons <<= 4;
        buttons |= (buf[5] >> 4);
        uint16_t changes = (buttons ^ oldButtons);

        if (changes && joyEvents) {
            for (uint8_t i = 0; i < 0x0C; i++) {
                uint16_t mask = (0x0001 << i);
                if ((mask & changes) > 0) {
                    if ((buttons & mask) > 0)
                        joyEvents->OnButtonDn(i + 1);
                    else
                        joyEvents->OnButtonUp(i + 1);
                }
            }
            oldButtons = buttons;
        }
    }
};

#endif // __HIDJOYSTICKRPTPARSER_H__
