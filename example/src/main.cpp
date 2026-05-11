#define USING_RESISTOR_LADDER 0

#include <stdio.h>

#include <hardware/watchdog.h>

#include <display/SSD1306.h>

#include <interactive-ui/ScreenManager.h>
#include <interactive-ui/components/PaddingComponent.h>
#include <interactive-ui/components/BitmapComponent.h>
#include <interactive-ui/components/PixelBufferComponent.h>

#include <components/SettingsCategoryComponent.h>
#include <components/ClockComponent.h>

#include <hardware/Button.h>
#include <hardware/Timer.h>
#include <hardware/ResistorLadder.h>

#include <Icons.h>

constexpr Vec2u32 screen_dimensions = {128, 64};

struct _init
{
    _init()
    {
        stdio_init_all();
        i2c_init(i2c0, 100000);
        i2c_init(i2c1, 600000);
        adc_init();
    }
} _init_inst;

SSD1306 display(2, 3, i2c1);

ScreenManager manager(&display);
Screen screen1(&manager, screen_dimensions);
Screen screen2(&manager, screen_dimensions);
Screen screen3(&manager, screen_dimensions);

TextComponent text1(&manager, Vec2i32{16, 20}, "0", &fonts::default_font, 1, &screen1);
TextComponent text2(&manager, Vec2i32{16, 32}, "1", &fonts::default_font, 2, &screen1);
TextComponent text3(&manager, Vec2i32{32, 20}, "2", &fonts::default_font, 3, &screen1);
TextComponent text4(&manager, Vec2i32{32, 32}, "3", &fonts::default_font, 4, &screen1);
PaddingComponent padding(&manager, Vec2i32{0, 16}, {127, 63 - 16}, 3, &screen1);
TextBoxComponent hidden_message(&manager, Vec2i32{64, 32}, Vec2i32{64, 32}, "Message", &fonts::default_font, 3, &screen1);

TextBoxComponent screen2_message(&manager, Vec2i32{0, 0}, Vec2i32{127, 63}, "Screen 2", &fonts::default_font, 0, &screen2);

PaddingComponent settings_padding(&manager, Vec2i32{0, 0}, Vec2i32{127, 63}, 0, &screen3);
TextComponent settings_back(&manager, Vec2i32{4, 4}, "Back", &fonts::default_font, 2, &screen3);
SettingsCategoryComponent settings_display(&manager, Vec2i32{4, 20}, Vec2i32{50, 11}, "Display", &fonts::default_font, -1, &screen3);
SettingsCategoryComponent settings_volume(&manager, Vec2i32{4, 40}, Vec2i32{50, 11}, "Volume", &fonts::default_font, -1, &screen3);
SettingsCategoryComponent settings_clock(&manager, Vec2i32{4, 60}, Vec2i32{50, 11}, "Clock", &fonts::default_font, -1, &screen3);
SettingsCategoryComponent settings_restart(&manager, Vec2i32{4, 80}, Vec2i32{50, 11}, "Restart", &fonts::default_font, -1, &screen3);

char vol_txt[4];

Screen screen_volume_control(&manager, screen_dimensions);
TextBoxComponent volume_control_value(&manager, Vec2i32{24, 26}, Vec2i32{20, 11}, vol_txt, &fonts::default_font, 3, &screen_volume_control);
BitmapComponent volume_increment(&manager, Vec2i32{32, 20}, increment_icon, 3, &screen_volume_control);
BitmapComponent volume_decrement = volume_increment;
TextComponent volume_screen_name(&manager, Vec2i32{24, 4}, "Volume Control", &fonts::default_font, 0, &screen_volume_control);
TextBoxComponent volume_variable_checkbox(&manager, Vec2i32{80, 28}, Vec2i32{7, 7}, "Variable\n\nVolume", &fonts::default_font, 0, &screen_volume_control);
PixelBufferComponent volume_variable_checkbox_check(&manager, Vec2i32{81, 29}, x_icon, 1, &screen_volume_control);

Screen screen_clock_control(&manager, screen_dimensions);
TextBoxComponent hour_control(&manager, Vec2f{0.4f, 0.5f}, Vec2i32{15, 13}, ClockComponent::cc_hour, &fonts::default_font, 1, &screen_clock_control);
TextBoxComponent minute_control(&manager, Vec2f{0.6f, 0.5f}, Vec2i32{15, 13}, ClockComponent::cc_minute, &fonts::default_font, 1, &screen_clock_control);
TextComponent time_control_colon(&manager, Vec2f{0.5f, 0.5f}, ":", &fonts::default_font, 1, &screen_clock_control);
TextComponent time_control_am_pm(&manager, Vec2f{0.75f, 0.5f}, ClockComponent::cc_am_pm, &fonts::default_font, 1, &screen_clock_control);
BitmapComponent hour_increment(&manager, Vec2i32{0, 0}, increment_icon, 1, &screen_clock_control); // set pos later
BitmapComponent hour_decrement = hour_increment;
BitmapComponent minute_increment = hour_increment;
BitmapComponent minute_decrement = hour_increment;
TextBoxComponent clock_control_confirm(&manager, Vec2f{0.5f, 0.75f}, Vec2i32{53, 13}, "Confirm", &fonts::default_font, 1, &screen_clock_control);
TextBoxComponent clock_change_time_fmt(&manager, Vec2f{0.5f, 0.15f}, Vec2i32{7, 7}, "  24  Hour", &fonts::default_font, 1, &screen_clock_control);
PixelBufferComponent clock_change_time_fmt_check(&manager, Vec2f{0.5f, 0.15f}, x_icon, 2, &screen_clock_control);

bool lpm = false;
bool screen_pwr = true;
BitmapComponent battery(&manager, Vec2i32{126, 2}, battery_icon, 10);
TextComponent battery_lpm_message(&manager, battery.GetOriginPosition() + Vec2i32{-20, 2}, "LPM", &fonts::default_font, 10);
CountdownTimer lpm_sleep_timer;

uint8_t volume_percentage = 50;
BitmapComponent speaker(&manager, Vec2i32{0, 0}, speaker_icon, 5, &screen1);
PixelBufferComponent vol_mute(&manager, Vec2i32{12, 5}, x_icon, 6, &screen1);
PixelBufferComponent vol_lvl_1(&manager, Vec2i32{12, 4}, volume_level_1_icon, 5, &screen1);
PixelBufferComponent vol_lvl_2(&manager, Vec2i32{13, 2}, volume_level_2_icon, 5, &screen1);
PixelBufferComponent vol_lvl_3(&manager, Vec2i32{14, 0}, volume_level_3_icon, 5, &screen1);

TextBoxComponent volume_indicator(&manager, Vec2i32{63, -20}, Vec2i32{32, 12}, vol_txt, &fonts::default_font, INT32_MAX, &screen1);
MovementAnimation volume_ind_move(&volume_indicator, easing::lut_quad_out);
CountdownTimer volume_move_timer;

void button_callback_generic(const Event* ev, uint32_t control_mask, const char* name = "")
{
#if USING_RESISTOR_LADDER
    AnalogEvent* event = (AnalogEvent*)ev;
    printf("ADC Val: %d\n", event->GetADCValue());
#else
    ButtonEvent* event = (ButtonEvent*)ev;
    if (event->WasPressed())
    {
#endif

    if (!screen_pwr)
    {
        display.Power(true);
        screen_pwr = true;
        if (lpm)
            lpm_sleep_timer.Start(5000);
    
        return;
    }

    manager.UpdateDeltaTime(); // eat large previous delta if any

    manager.QueueControl(control_mask);
    printf("%s Button Pressed.\n", name);

    manager.Update();

    if (lpm)
    {
        lpm_sleep_timer.Start(5000);
    }

#if !USING_RESISTOR_LADDER
    }
#endif
}    

void select_button_callback(const Event* ev, void*)
{
    button_callback_generic(ev, ControlAction::SELECT0, "Select");
}

void back_button_callback(const Event* ev, void*)
{
    button_callback_generic(ev, ControlAction::BACK, "Back");
}

void left_button_callback(const Event* ev, void*)
{
    button_callback_generic(ev, ControlAction::DIRECTIONAL_LEFT, "Left");
}

void right_button_callback(const Event* ev, void*)
{
    button_callback_generic(ev, ControlAction::DIRECTIONAL_RIGHT, "Right");
}

void up_button_callback(const Event* ev, void*)
{
    button_callback_generic(ev, ControlAction::DIRECTIONAL_UP, "Up");
}

void down_button_callback(const Event* ev, void*)
{
    button_callback_generic(ev, ControlAction::DIRECTIONAL_DOWN, "Down");
}

void update_volume_graphic()
{
    snprintf(vol_txt, 4, "%u", volume_percentage);

    if (volume_percentage == 0)
    {
        vol_mute.SetPersonalVisibility(true);
        vol_lvl_1.SetPersonalVisibility(false);
        vol_lvl_2.SetPersonalVisibility(false);
        vol_lvl_3.SetPersonalVisibility(false);
    }
    else if (volume_percentage < 25)
    {
        vol_mute.SetPersonalVisibility(false);
        vol_lvl_1.SetPersonalVisibility(false);
        vol_lvl_2.SetPersonalVisibility(false);
        vol_lvl_3.SetPersonalVisibility(false);
    }
    else if (volume_percentage < 50)
    {
        vol_mute.SetPersonalVisibility(false);
        vol_lvl_1.SetPersonalVisibility(true);
        vol_lvl_2.SetPersonalVisibility(false);
        vol_lvl_3.SetPersonalVisibility(false);
    }
    else if (volume_percentage < 75)
    {
        vol_mute.SetPersonalVisibility(false);
        vol_lvl_1.SetPersonalVisibility(true);
        vol_lvl_2.SetPersonalVisibility(true);
        vol_lvl_3.SetPersonalVisibility(false);
    }
    else
    {
        vol_mute.SetPersonalVisibility(false);
        vol_lvl_1.SetPersonalVisibility(true);
        vol_lvl_2.SetPersonalVisibility(true);
        vol_lvl_3.SetPersonalVisibility(true);
    }
}

void move_volume_box()
{
    if (manager.GetCurrentScreen() != &screen1) // only allow the animation on screen 1
        return;

    if (!volume_indicator.IsMoving() && volume_indicator.GetOriginPosition() != volume_ind_move.end_pos)
    {
        if (lpm)
            volume_ind_move.duration = 0.f;
        else
            volume_ind_move.duration = 1.f;

        volume_indicator.Move(volume_ind_move);
    }
    else
        volume_move_timer.Start(1500);
}

void volume_change(bool increase)
{
    if (increase && volume_percentage < 100)
        volume_percentage++;
    else if (!increase && volume_percentage > 0)
        volume_percentage--;

    update_volume_graphic();
    move_volume_box();

    manager.Update();
}

void vol_down_button_callback(const Event* ev, void*)
{
#if !USING_RESISTOR_LADDER
    if (ev->GetEventAsType<ButtonEvent>()->WasPressed())
    {
#endif

    printf("Volume Down: %d\n", volume_percentage);
    
    volume_change(false);

#if !USING_RESISTOR_LADDER
    }
#endif
}

void vol_up_button_callback(const Event* ev, void*)
{
#if !USING_RESISTOR_LADDER
    if (ev->GetEventAsType<ButtonEvent>()->WasPressed())
    {
#endif
    
    printf("Volume Up: %d\n", volume_percentage);
    
    volume_change(true);

#if !USING_RESISTOR_LADDER
    }
#endif
}

int main()
{
    puts("Program start.");

    ClockComponent clock(&manager, Vec2i32{63, 2}, 4, 5, 6, &fonts::default_font, -1, &screen1);

    puts("Objects created.");

    display.ClearDisplay();
    manager.EnableCBF(true);

    puts("Display Cleared.");

    // The initial screen was already given, so this overwrites the LUT entry.
    text1.AddComponentTable(&screen1, &clock, &text2, nullptr, &text3);
    text2.AddComponentTable(&screen1, &text1, nullptr, nullptr, &text4);
    text3.AddComponentTable(&screen1, &clock, &text4, &text1, nullptr);
    text4.AddComponentTable(&screen1, &text3, nullptr, &text2, nullptr);

    // Settings example screen
    settings_back.AddComponentTable(&screen3, nullptr, &settings_display, nullptr, &battery);
    settings_display.AddComponentTable(&screen3, &settings_back, &settings_volume);
    settings_volume.AddComponentTable(&screen3, &settings_display, &settings_clock);
    settings_clock.AddComponentTable(&screen3, &settings_volume, &settings_restart);
    settings_restart.AddComponentTable(&screen3, &settings_clock);

    // Since we gave a nullptr as the screen, we must construct a table for each screen using the component.
    battery.AddComponentTable(&screen1, nullptr, &text3, &clock, nullptr);
    battery.AddComponentTable(&screen2);
    battery.AddComponentTable(&screen3, nullptr, &settings_display, &settings_back, nullptr);

    clock.AddComponentTable(&screen1, nullptr, &text3, nullptr, &battery);

    volume_control_value.AddComponentTable(&screen_volume_control, nullptr, nullptr, nullptr, &volume_variable_checkbox);
    volume_variable_checkbox.AddComponentTable(&screen_volume_control, nullptr, nullptr, &volume_control_value, nullptr);

    clock_change_time_fmt.AddComponentTable(&screen_clock_control, nullptr, &clock_control_confirm, &hour_control, &minute_control);
    clock_control_confirm.AddComponentTable(&screen_clock_control, &clock_change_time_fmt, nullptr, &hour_control, &minute_control);
    hour_control.AddComponentTable(&screen_clock_control, &clock_change_time_fmt, &clock_control_confirm, nullptr, &minute_control);
    minute_control.AddComponentTable(&screen_clock_control, &clock_change_time_fmt, &clock_control_confirm, &hour_control, nullptr);

    puts("Component tables created.");

    screen1.AddComponent(&battery);
    screen1.AddComponent(&battery_lpm_message);

    screen2.AddComponent(&battery);
    screen2.AddComponent(&battery_lpm_message);

    screen3.AddComponent(&battery);
    screen3.AddComponent(&battery_lpm_message);

    screen_volume_control.AddComponent(&settings_padding);
    screen_clock_control.AddComponent(&settings_padding);

    puts("Added components.");

    hidden_message.SetTextAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);

    battery.SetHorizontalAlignment(AlignmentHorizontal::RIGHT);
    battery_lpm_message.SetHorizontalAlignment(AlignmentHorizontal::RIGHT);

    clock.SetHorizontalAlignment(AlignmentHorizontal::CENTER);

    volume_indicator.SetHorizontalAlignment(AlignmentHorizontal::CENTER);
    volume_indicator.SetTextHorizontalAlignment(AlignmentHorizontal::CENTER);

    volume_variable_checkbox.SetTextHorizontalAlignment(AlignmentHorizontal::CENTER);

    hour_control.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    minute_control.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    hour_increment.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    hour_decrement.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    minute_increment.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    minute_decrement.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    time_control_colon.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    time_control_am_pm.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);

    clock_control_confirm.SetHorizontalAlignment(AlignmentHorizontal::CENTER);
    clock_control_confirm.SetTextAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    hour_control.SetTextAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    minute_control.SetTextAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    clock_change_time_fmt.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);
    clock_change_time_fmt.SetTextHorizontalAlignment(AlignmentHorizontal::CENTER);
    clock_change_time_fmt_check.SetAlignment(AlignmentVertical::CENTER, AlignmentHorizontal::CENTER);

    puts("Aligned components.");

    // Sorts the components by Z-layer. Lowest Z-layer components drawn first.
    // It also chooses the highest Z-layer selectable component by default.
    screen1.SortComponents();
    screen2.SortComponents();
    screen3.SortComponents();

    puts("Sorted components.");

    screen1.HoverComponent(&text1, true);
    // no screen 2 hover
    screen3.HoverComponent(&settings_back, true);
    screen_volume_control.HoverComponent(&volume_control_value, true);
    screen_clock_control.HoverComponent(&hour_control, true);

    puts("Set the initial hovered components.");

    volume_indicator.clear_bg = true;

    volume_decrement.SetOriginPosition(volume_increment.GetOriginPosition() + Vec2i32{0, 20});
    volume_decrement.mirror_vertically = true;

    hour_increment.SetOriginPosition(hour_control.GetOriginPosition() - Vec2i32{0, 12});
    hour_decrement.SetOriginPosition(hour_increment.GetOriginPosition() + Vec2i32{0, 24});
    hour_decrement.mirror_vertically = true;
    minute_increment.SetOriginPosition(minute_control.GetOriginPosition() - Vec2i32{0, 12});
    minute_decrement.SetOriginPosition(minute_increment.GetOriginPosition() + Vec2i32{0, 24});
    minute_decrement.mirror_vertically = true;

    volume_variable_checkbox.SetPadding(Vec2i32{-20, -10});
    clock_change_time_fmt.SetPadding(Vec2i32{-20, 0});

    puts("Components are setup.");

    volume_ind_move.start_pos = volume_indicator.GetOriginPosition();
    volume_ind_move.end_pos = Vec2i32{63, 2};
    volume_ind_move.duration = 1.f;

    volume_ind_move.on_animation_end = [](const MovementAnimation*){
        volume_move_timer.Start(1500);
    };

    int id_vol_move = volume_move_timer.AddAction([](const Event*, void*){
        if (lpm)
            volume_ind_move.duration = 0.f;
        else
            volume_ind_move.duration = 1.f;

        volume_indicator.Move(volume_ind_move, true, false);
    });

    puts("Animations are setup.");

    /**
     * Hidden components by default
     */
    hidden_message.SetPersonalVisibility(false);
    battery_lpm_message.SetPersonalVisibility(lpm); // dependent on lpm (could be initialized in config or something)
    update_volume_graphic(); // handles the hiding for us
    volume_variable_checkbox_check.SetPersonalVisibility(false);
    clock_change_time_fmt_check.SetPersonalVisibility(false);

    puts("Initially hidden components are hidden.");

    manager.PushScreen(&screen1); // Default screen.

    puts("Pushed screen.");

    
    // Use the event system
    RepeatingTimer vol_up_repeat(25);
    RepeatingTimer vol_down_repeat(25);
    RepeatingTimer right_button_repeat(50);
    RepeatingTimer left_button_repeat(50);
    RepeatingTimer down_button_repeat(50);
    RepeatingTimer up_button_repeat(50);
    
    // this is safe because we are not accessing the event as a specific type
    int id_vol_up_repeat = vol_up_repeat.AddAction(&vol_up_button_callback);
    int id_vol_down_repeat = vol_down_repeat.AddAction(&vol_down_button_callback);
    int id_right_repeat = right_button_repeat.AddAction(&right_button_callback);
    int id_left_repeat = left_button_repeat.AddAction(&left_button_callback);
    int id_down_repeat = down_button_repeat.AddAction(&down_button_callback);
    int id_up_repeat = up_button_repeat.AddAction(&up_button_callback);

#if USING_RESISTOR_LADDER

    AnalogRepeatingDevice vol_up_button(26, 1400, vol_up_repeat, 500, 250);
    AnalogRepeatingDevice vol_down_button(26, 2000, vol_down_repeat, 500, 250);

    AnalogDevice back_button(26, 2500);
    AnalogDevice select_button(26, 2750);

    AnalogRepeatingDevice right_button(26, 3100, right_button_repeat, 500, 100);
    AnalogRepeatingDevice left_button(26, 3375, down_button_repeat, 500, 100);
    AnalogRepeatingDevice down_button(26, 3525, up_button_repeat, 500, 100);
    AnalogRepeatingDevice up_button(26, 3800, left_button_repeat, 500, 100);

#else

    //RepeatingButton vol_up_button(18, vol_down_repeat, 500);
    //RepeatingButton vol_down_button(21, vol_down_repeat, 500);
    Button vol_up_button(18, true, 50);
    Button vol_down_button(21, true, 50);

    Button back_button(17, true, 100);
    Button select_button(20, true, 100);

    //RepeatingButton right_button(15, right_button_repeat, 500);
    //RepeatingButton left_button(13, left_button_repeat, 500);
    //RepeatingButton down_button(14, down_button_repeat, 500);
    //RepeatingButton up_button(12, up_button_repeat, 500);
    Button right_button(15, true, 100);
    Button left_button(13, true, 100);
    Button down_button(14, true, 100);
    Button up_button(12, true, 100);

#endif

    puts("I/O Devices created.");

    int id_up_button = up_button.AddAction(&up_button_callback);
    int id_down_button = down_button.AddAction(&down_button_callback);
    int id_left_button = left_button.AddAction(&left_button_callback);
    int id_right_button = right_button.AddAction(&right_button_callback);
    int id_select_button = select_button.AddAction(&select_button_callback);
    int id_back_button = back_button.AddAction(&back_button_callback);
    int id_vol_down_button = vol_down_button.AddAction(&vol_down_button_callback);
    int id_vol_up_button = vol_up_button.AddAction(&vol_up_button_callback);

    puts("Callbacks linked.");

#if USING_RESISTOR_LADDER

    DetectableResistorLadder<8> rladder(16, Pull::UP, GPIO_IRQ_EDGE_FALL);
    rladder[0] = &vol_up_button;
    rladder[1] = &vol_down_button;
    rladder[2] = &back_button;
    rladder[3] = &select_button;
    rladder[4] = &right_button;
    rladder[5] = &left_button;
    rladder[6] = &down_button;
    rladder[7] = &up_button;

    // Resistor ladder doesn't require an IRQ callback, it instead wraps the analog devices
    rladder.SortDevices();

    puts("Resistor ladder initialized.");

#endif

    CountdownTimer hidden_message_timer;

    int id_selecttext1 = text1.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
        {
            hidden_message.SetPersonalVisibility(true);
            CountdownTimer* timer = (CountdownTimer*)ptr;
            timer->Start(3000);
        }
    }, &hidden_message_timer);

    int id_hidden_message_timer = hidden_message_timer.AddAction([](const Event*, void*){
        hidden_message.SetPersonalVisibility(false);
        manager.Update();
    });

    int id_selecttext2 = text2.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
            manager.PushScreen(&screen2);
    });

    int id_selecttext3 = text3.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
            manager.PushScreen(&screen3);
    });

    int id_selecttext4 = text4.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
        {
            MovementAnimation animation(&text4, easing::lut_quad_in_out); // this is the coolest thing ever it ACTUALLY WORKS
            animation.duration = 1.f;
            animation.end_pos = Vec2i32{100, 40};
            text4.Move(animation);
        }
    });

    int id_screen3_back = settings_back.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
        {
            manager.QueueControl(ControlAction::BACK);
        }
    });

    int id_battery = battery.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
        {
            lpm = !lpm;
            battery_lpm_message.SetPersonalVisibility(lpm);
            if (lpm)
            {
                lpm_sleep_timer.Start(5000);
                screen1.animation_hover_duration = 0.f;
                screen2.animation_hover_duration = 0.f;
                screen3.animation_hover_duration = 0.f;
                screen_volume_control.animation_hover_duration = 0.f;
                screen_clock_control.animation_hover_duration = 0.f;
            }
            else
            {
                lpm_sleep_timer.End();
                screen1.animation_hover_duration = 0.25f;
                screen2.animation_hover_duration = 0.25f;
                screen3.animation_hover_duration = 0.25f;
                screen_volume_control.animation_hover_duration = 0.25f;
                screen_clock_control.animation_hover_duration = 0.25f;
            }
        }
    });

    int id_clock_select = clock.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        ClockComponent* clock = (ClockComponent*)ptr;
        if (event->GetControl() == ControlAction::SELECT0)
        {
            clock->UpdateTimeSlots();
            manager.PushScreen(&screen_clock_control);
        }
    }, &clock);

    int id_sleep = lpm_sleep_timer.AddAction([](const Event*, void*){
        if (lpm) // failsafe
        {
            display.Power(false);
            screen_pwr = false;
        }
    });

    int id_settings_volume_tab = settings_volume.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
        {
            manager.PushScreen(&screen_volume_control);
        }
    });

    int is_settings_restart_tab = settings_restart.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
        {
            watchdog_enable(1, 1);
            while (1) {}
        }
    });

    int id_volume_increment_box = volume_control_value.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        switch (event->GetControl())
        {
            case DIRECTIONAL_UP: return volume_change(true);
            case DIRECTIONAL_DOWN: return volume_change(false);
            default: return;
        }
    });

    int id_volume_variable_checkbox = volume_variable_checkbox.AddAction([](const Event* ev, void*){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        if (event->GetControl() == ControlAction::SELECT0)
        {
            volume_variable_checkbox_check.SetPersonalVisibility(!volume_variable_checkbox_check.GetPersonalVisibility());
        }
    });

    int id_settings_clock_tab = settings_clock.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        ClockComponent* clock = (ClockComponent*)ptr;
        if (event->GetControl() == ControlAction::SELECT0)
        {
            clock->UpdateTimeSlots();
            manager.PushScreen(&screen_clock_control);
        }
    }, &clock);

    int id_clock_confirm = clock_control_confirm.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        ClockComponent* clock = (ClockComponent*)ptr;
        clock->GetClock().UpdateDateAndTime();
        auto data = clock->GetClock().GetDateAndTime();
        data.hours = ClockComponent::hour_count;
        data.minutes = ClockComponent::minute_count;
        data.am_pm = ClockComponent::am_pm;
        if (event->GetControl() == ControlAction::SELECT0)
        {
            clock->GetClock().SetTime(data);
            manager.PopScreen();
        }
    }, &clock);

    int id_clock_24hour = clock_change_time_fmt.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        ClockComponent* clock = (ClockComponent*)ptr;
        if (event->GetControl() == ControlAction::SELECT0)
        {
            bool checked = clock_change_time_fmt_check.GetPersonalVisibility();
            clock_change_time_fmt_check.SetPersonalVisibility(!checked);
            clock->GetClock().Use24HourTime(!checked);
        } 
    }, &clock);

    int id_hour_edit = hour_control.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        ClockComponent* clock = (ClockComponent*)ptr;
        uint64_t control = event->GetControl();
        if (control == ControlAction::SELECT0)
        {
            hour_control.LockToggle();
        }
        if (hour_control.IsLocked())
        {
            switch (control)
            {
                case DIRECTIONAL_UP:

                    ClockComponent::hour_count++;
                    if (clock->GetClock().Is24HourTime())
                    {
                        if (ClockComponent::hour_count == 24)
                            ClockComponent::hour_count = 0;
                        
                        snprintf(ClockComponent::cc_hour, 3, "%02i", ClockComponent::hour_count);
                    }
                    else
                    {
                        if (ClockComponent::hour_count == 13)
                        {
                            ClockComponent::am_pm = !ClockComponent::am_pm;
                            ClockComponent::hour_count = 1;
                        }
                        snprintf(ClockComponent::cc_hour, 3, "%2i", ClockComponent::hour_count);
                    }
                    break;
                case DIRECTIONAL_DOWN:
                    ClockComponent::hour_count--;
                    if (clock->GetClock().Is24HourTime())
                    {
                        if (ClockComponent::hour_count == -1)
                            ClockComponent::hour_count = 23;

                        snprintf(ClockComponent::cc_hour, 3, "%02i", ClockComponent::hour_count);
                    }
                    else
                    {
                        if (ClockComponent::hour_count == 0)
                        {
                            ClockComponent::am_pm = !ClockComponent::am_pm;
                            ClockComponent::hour_count = 12; 
                        }
                        snprintf(ClockComponent::cc_hour, 3, "%2i", ClockComponent::hour_count);
                    }
                    break;
                default : break;
            }
        }
    }, &clock);

    int id_minute_edit = minute_control.AddAction([](const Event* ev, void* ptr){
        ComponentSelectEvent* event = ev->GetEventAsType<ComponentSelectEvent>();
        uint64_t control = event->GetControl();
        if (control == ControlAction::SELECT0)
        {
            minute_control.LockToggle();
        }
        if (minute_control.IsLocked())
        {
            switch (control)
            {
                case DIRECTIONAL_UP:
                    ClockComponent::minute_count++;
                    if (ClockComponent::minute_count > 59)
                        ClockComponent::minute_count = 0;
                
                    break;
                case DIRECTIONAL_DOWN:
                    ClockComponent::minute_count--;
                    if (ClockComponent::minute_count < 0)
                        ClockComponent::minute_count = 59;

                    break;
                default : break;
            }
            snprintf(ClockComponent::cc_minute, 3, "%02i", ClockComponent::minute_count);
        }
    });

    puts("Actions set.");

    manager.Update();

    puts("Screen manager updated, entering loop:");


    while (1)
    {
        Event::HandleEvents();
        manager.UpdateDeltaTime();
        manager.UpdateIfAnyComponentMoving();
        manager.UpdateIfCursorActivity();
    }
}