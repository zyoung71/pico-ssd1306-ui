#pragma once

#include <interactive-ui/components/TextComponent.h>
#include <rtc/RealTimeClock.h>

void update_time_slots();

class ClockComponent : public TextComponent
{
public:
    static char cc_hour[3];
    static char cc_minute[3];
    static char cc_am_pm[3];
    static int8_t hour_count;
    static int8_t minute_count;
    static bool am_pm;    

protected:
    RealTimeClock clock;

public:
    ClockComponent(ScreenManager* manager, const Vec2i32& origin, uint8_t sda, uint8_t scl, uint8_t interrupt, const Font* font, int z_layer, Screen* initial_screen = nullptr);

    void Update(float dt, const Screen* screen) override;
    
    inline RealTimeClock& GetClock()
    {
        return clock;
    }

    void UpdateTimeSlots();
};