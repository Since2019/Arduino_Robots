#ifndef __HIDJOYSTICKRPTPARSER_H__
#define __HIDJOYSTICKRPTPARSER_H__

#include <usbhid.h>

// 手柄数据包结构（根据你手柄数据格式定义）
struct GamePadEventData {
    uint8_t reportId; // 第1字节：报告ID，0x01 表示主输入
    uint8_t dummy2;   // 未使用
    uint8_t dummy3;   // 未使用
    uint8_t X;        // 左摇杆 X
    uint8_t Y;        // 左摇杆 Y
    uint8_t Z;        // 暂未使用（你可以扩展）
};

class JoystickEvents {
    bool changed = false;
    uint8_t joyX = 127;
    uint8_t joyY = 127;
    uint16_t buttons = 0; // ✅ 保存按钮状态

public:
    // 主动更新按钮状态（供Parser内部调用）
    void updateButtons(uint8_t newBtnBits) {
        buttons &= 0xFFF0;          // 清除低4位
        buttons |= (newBtnBits & 0x0F); // 设置新的低4位
    }
    
    void OnGamePadChanged(const GamePadEventData *evt) {
        if (evt->reportId == 0x01) {
            // 只有当值真的变化才标记 changed
            if (joyX != evt->X || joyY != evt->Y) {
                joyX = evt->X;
                joyY = evt->Y;
                changed = true;
            }
        }
    }

    void OnHatSwitch(uint8_t hat) {}
    
    void OnButtonUp(uint8_t but_id) {
        buttons &= ~(1 << (but_id - 1));  // 清除该位
    }

    void OnButtonDn(uint8_t but_id) {
        buttons |= (1 << (but_id - 1));  // 设置该位
    }

    uint8_t getX() const { return joyX; }
    uint8_t getY() const { return joyY; }
    uint16_t getButtons() const { return buttons; }  // ✅ 提供按钮状态读取

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
    uint8_t oldButtons; // 只记录低4位的按钮状态（1~4）

public:
    JoystickReportParser(JoystickEvents *evt)
        : joyEvents(evt), oldHat(0xDE), oldButtons(0) {
        memset(oldPad, 0xD, sizeof(oldPad));
    }

    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
//  //       打印原始数据包内容（调试用）
//        Serial.print("Raw HID Packet: ");
//        for (uint8_t i = 0; i < len; i++) {
//            if (buf[i] < 0x10) Serial.print("0");
//            Serial.print(buf[i], HEX);
//            Serial.print(" ");
//        }
//        Serial.println();

        // 数据变更才处理
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

        // Hat（方向键）处理（未使用）
        uint8_t hat = (buf[5] & 0x0F);
        if (hat != oldHat && joyEvents) {
            joyEvents->OnHatSwitch(hat);
            oldHat = hat;
        }

        // 处理右边 XYAB 按钮（高4位）
        uint8_t btnBits = buf[5] >> 4; // 高4位，代表按钮1~4
        uint8_t changes = btnBits ^ oldButtons;

        if (changes && joyEvents) {
            Serial.println("joyEvents");
            for (uint8_t i = 0; i < 4; i++) {
                uint8_t mask = 1 << i;
                if (changes & mask) {
                    if (btnBits & mask)
                        joyEvents->OnButtonDn(i + 1);
                    else
                        joyEvents->OnButtonUp(i + 1);
                }
            }
            oldButtons = btnBits;
            joyEvents->updateButtons(btnBits); // 更新按钮状态
        }
    }
};

#endif // __HIDJOYSTICKRPTPARSER_H__
