//Derived from peerdavid's source at: https://github.com/peerdavid/Watchy
#ifndef WATCHY_BASE_H
#define WATCHY_BASE_H

#include <Watchy.h>
#define SLEEP_HOUR          1
#define SLEEP_MINUTE        0

#if __has_include("config.h") && __has_include(<stdint.h>)
# include "config.h"
#endif

extern RTC_DATA_ATTR bool twelve_mode;
extern RTC_DATA_ATTR int16_t alarm_timer;

// Btn definitions
#define IS_DOUBLE_TAP       (wakeupBit & ACC_INT_MASK && guiState == WATCHFACE_STATE)
#define IS_BTN_RIGHT_UP     (wakeupBit & UP_BTN_MASK && guiState == WATCHFACE_STATE)
#define IS_BTN_LEFT_UP      (wakeupBit & BACK_BTN_MASK && guiState == WATCHFACE_STATE)
#define IS_BTN_RIGHT_DOWN   (wakeupBit & DOWN_BTN_MASK && guiState == WATCHFACE_STATE)
#define EXT_INT_MASK        MENU_BTN_MASK|BACK_BTN_MASK|UP_BTN_MASK|DOWN_BTN_MASK

class WatchyBase : public Watchy {
    public:
        WatchyBase();
        virtual void init();
        virtual void handleButtonPress();
        virtual void deepSleep();
        void vibrate(uint8_t times=1, uint32_t delay_time=50);
        bool watchFaceDisabled();
        esp_sleep_wakeup_cause_t wakeup_reason;
    private:
        void _rtcConfig();
};

#endif
