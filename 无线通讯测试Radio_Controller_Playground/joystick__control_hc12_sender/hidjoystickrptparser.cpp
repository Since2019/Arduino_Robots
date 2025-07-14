#include "hidjoystickrptparser.h"

JoystickReportParser::JoystickReportParser(JoystickEvents *evt)
  : joyEvents(evt), oldHat(0xDE), oldButtons(0) {
    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
        oldPad[i] = 0xD;
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    bool match = true;
    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
        if (buf[i] != oldPad[i]) {
            match = false;
            break;
        }

    if (!match && joyEvents) {
        joyEvents->OnGamePadChanged((const GamePadEventData*)buf);
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = buf[i];
    }
}
