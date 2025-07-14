#include "hidjoystickrptparser.h"

JoystickReportParser::JoystickReportParser(JoystickEvents *evt)
  : joyEvents(evt), oldHat(0xDE), oldButtons(0) {
    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
        oldPad[i] = 0xD;
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {

    Serial.print("Raw Report: ");
    for (uint8_t i = 0; i < len; i++) {
        Serial.print(buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    
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

    uint8_t hat = (buf[5] & 0xF);
    if (hat != oldHat && joyEvents) {
        joyEvents->OnHatSwitch(hat);
        oldHat = hat;
    }

    uint16_t buttons = (0x0000 | buf[6]);
    buttons <<= 4;
    buttons |= (buf[5] >> 4);
    uint16_t changes = (buttons ^ oldButtons);

    if (changes) {
        for (uint8_t i = 0; i < 0x0C; i++) {
            uint16_t mask = (0x0001 << i);
            if (((mask & changes) > 0) && joyEvents) {
                if ((buttons & mask) > 0)
                    joyEvents->OnButtonDn(i + 1);
                else
                    joyEvents->OnButtonUp(i + 1);
            }
        }
        oldButtons = buttons;
    }
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
    Serial.print("Hat Switch: ");
    Serial.println(hat, HEX);
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
    Serial.print("Button Up: ");
    Serial.println(but_id);
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
    Serial.print("Button Down: ");
    Serial.println(but_id);
}
