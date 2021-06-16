//Derived from peerdavid's source at: https://github.com/peerdavid/Watchy
#include "Watchy_Base.h"

RTC_DATA_ATTR int16_t alarm_timer = -1;
RTC_DATA_ATTR bool twelve_mode = true;
RTC_DATA_ATTR bool sleep_mode = false;

WatchyBase::WatchyBase(){}

void WatchyBase::init(){
    wakeup_reason = esp_sleep_get_wakeup_cause(); //get wake up reason
    Wire.begin(SDA, SCL); //init i2c

    switch (wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0: //RTC Alarm

            // Handle alarm
            if(RTC.alarm(ALARM_1) && alarm_timer >= 0){
                RTC.alarmInterrupt(ALARM_1, false); // disable interrupt

                vibrate(3, 500);
                alarm_timer = -1;

                // Continue to update watch face
            }

            // Handle classical tick
            RTC.alarm(ALARM_2); //resets the alarm flag in the RTC

            // Only for visualization and to ensure that alarm is not triggered
            // again and a gain as the alarm flag is internally set every time...
            if(alarm_timer > 0){
                alarm_timer--;
            }

            if(guiState == WATCHFACE_STATE){
                RTC.read(currentTime);

                if(currentTime.Hour == SLEEP_HOUR && currentTime.Minute == SLEEP_MINUTE){
                    sleep_mode = true;
                    RTC.alarmInterrupt(ALARM_2, false);
                }

                showWatchFace(true); //partial updates on tick
            }
            break;

        case ESP_SLEEP_WAKEUP_EXT1: //button Press + no handling if wakeup
            if(sleep_mode){
                sleep_mode = false;
                RTC.alarmInterrupt(ALARM_2, true);
                RTC.alarm(ALARM_2); //resets the alarm flag in the RTC

                RTC.read(currentTime);
                showWatchFace(false); //full update on wakeup from sleep mode
                break;
            }

            handleButtonPress();
            break;

        default: //reset
            _rtcConfig();
            showWatchFace(false); //full update on reset
            break;
    }

    deepSleep();
}

bool WatchyBase::watchFaceDisabled(){
    return sleep_mode;
}

void WatchyBase::deepSleep(){
  esp_sleep_enable_ext0_wakeup(RTC_PIN, 0); //enable deep sleep wake on RTC interrupt
  esp_sleep_enable_ext1_wakeup(EXT_INT_MASK, ESP_EXT1_WAKEUP_ANY_HIGH); //enable deep sleep wake on button press
  esp_deep_sleep_start();
}

void WatchyBase::handleButtonPress(){
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();

    if (IS_DOUBLE_TAP){
        while(!sensor.getINT()){
            // Wait until interrupt is cleared.
            // Otherwise it will fire again and again.
        }

        // To be defined in the watch face what we want exactly
        // to do. Therefore, no return;
    }

    if (IS_BTN_LEFT_UP){
        twelve_mode = (twelve_mode == 0) ? true:false;
        RTC.read(currentTime);
        vibrate();
        showWatchFace(true);
        return;
    }

    if(IS_BTN_RIGHT_UP){
        RTC.read(currentTime);
        vibrate();
        showWatchFace(true);
        return;
    }

    if(IS_BTN_RIGHT_DOWN){
        RTC.read(currentTime);
        vibrate();
        showWatchFace(true);
        return;
    }

    Watchy::handleButtonPress();
}

void WatchyBase::vibrate(uint8_t times, uint32_t delay_time){
    // Ensure that no false positive double tap is produced
    sensor.enableFeature(BMA423_WAKEUP, false);

    pinMode(VIB_MOTOR_PIN, OUTPUT);
    for(uint8_t i=0; i<times; i++){
        delay(delay_time);
        digitalWrite(VIB_MOTOR_PIN, true);
        delay(delay_time);
        digitalWrite(VIB_MOTOR_PIN, false);
    }

    sensor.enableFeature(BMA423_WAKEUP, true);
}

void WatchyBase::_rtcConfig(){
    //https://github.com/JChristensen/DS3232RTC
    RTC.squareWave(SQWAVE_NONE); //disable square wave output
    //RTC.set(compileTime()); //set RTC time to compile time
    RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 0); //alarm wakes up Watchy every minute
    RTC.alarmInterrupt(ALARM_2, true); //enable alarm interrupt
    RTC.read(currentTime);
}
