#include <components/ClockComponent.h>

char ClockComponent::cc_hour[3];
char ClockComponent::cc_minute[3];
char ClockComponent::cc_am_pm[3];
int8_t ClockComponent::hour_count = 0;
int8_t ClockComponent::minute_count = 0;
bool ClockComponent::am_pm = false;

ClockComponent::ClockComponent(ScreenManager* manager, const Vec2i32& origin, uint8_t sda, uint8_t scl, uint8_t interrupt, const Font* font, int z_layer, Screen* initial_screen)
    : TextComponent(manager, origin, nullptr, font, z_layer, initial_screen), clock(sda, scl, interrupt, i2c0)
{
    static constexpr ds3231_alarm_2_t discarded = {
        .minutes = 0,
        .hours = 0,
        .am_pm = 0,
        .day = 0,
        .date = 0
    };
    clock.UpdateDateAndTime();
    UpdateTimeSlots();

    // every minute, GPIO will be fired and screen will update the time
    clock.SetAlarm2(discarded, ON_EVERY_MINUTE);
    std::ignore = clock.AddAction([](const Event*, void* ptr){
        ScreenManager* mgr = (ScreenManager*)ptr;
        mgr->Update();
    }, manager);

    clock.Use24HourTime(false);
    SetText(clock.GetPrettyTime(RealTimeClock::TimeFormat::HH_MM));
}

void ClockComponent::Update(float dt, const Screen* screen)
{
    clock.UpdateDateAndTime();
    UpdateTimeSlots();

    clock.GetPrettyTime(RealTimeClock::TimeFormat::HH_MM);
    Align();
    TextComponent::Update(dt, screen);
}

void ClockComponent::UpdateTimeSlots()
{
    hour_count = clock.GetDateAndTime().hours;
    minute_count = clock.GetDateAndTime().minutes;
    am_pm = clock.GetDateAndTime().am_pm;
    if (clock.Is24HourTime())
    {
        snprintf(cc_hour, 3, "%2i", hour_count);
        strncpy(cc_am_pm, am_pm ? "PM" : "AM", 3);
    }
    else
    {
        snprintf(cc_hour, 3, "%02i", hour_count);
        strncpy(cc_am_pm, "\0", 3);
    }

    snprintf(cc_minute, 3, "%02i", minute_count);
}