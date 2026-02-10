#pragma once

#include <interactive-ui/components/TextComponent.h>
#include <rtc/RealTimeClock.h>

class ClockComponent : public TextComponent
{
protected:
    RealTimeClock clock;
    char* hour_ref;
    char* min_ref;

public:
    ClockComponent(ScreenManager* manager, const Vec2i32& origin, char* hour_ref, char* min_ref, uint8_t sda, uint8_t scl, uint8_t interrupt, const Font* font, int z_layer, Screen* initial_screen = nullptr);

    void Update(float dt, const Screen* screen) override;
    
    inline RealTimeClock* GetClock()
    {
        return &clock;
    }
};