#include <components/ClockComponent.h>

ClockComponent::ClockComponent(ScreenManager* manager, const Vec2i32& origin,  char* hour_ref, char* min_ref, uint8_t sda, uint8_t scl, uint8_t interrupt, const Font* font, int z_layer, Screen* initial_screen)
    : TextComponent(manager, origin, nullptr, font, z_layer, initial_screen), clock(sda, scl, interrupt, i2c0), hour_ref(hour_ref), min_ref(min_ref)
{
    static constexpr ds3231_alarm_2_t discarded = {
        .minutes = 0,
        .hours = 0,
        .am_pm = 0,
        .day = 0,
        .date = 0
    };
    clock.UpdateDateAndTime();
    snprintf(hour_ref, 3, "%s", clock.GetDateAndTime().hours);
    snprintf(min_ref, 3, "%s", clock.GetDateAndTime().minutes);

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
    snprintf(hour_ref, 3, "%s", clock.GetDateAndTime().hours);
    snprintf(min_ref, 3, "%s", clock.GetDateAndTime().minutes);
    clock.GetPrettyTime(RealTimeClock::TimeFormat::HH_MM);
    Align();
    TextComponent::Update(dt, screen);
}