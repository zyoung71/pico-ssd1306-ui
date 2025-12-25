#include <stdio.h>

#include <display/SSD1306.h>

#include <interactive-ui/ScreenManager.h>
#include <interactive-ui/components/TextComponent.h>
#include <interactive-ui/components/PaddingComponent.h>
#include <interactive-ui/components/IconComponent.h>

#include <hardware/Button.h>
#include <hardware/Timer.h>

#include <Icons.h>

constexpr Vec2u32 screen_dimensions = {128, 64};

struct _init
{
    _init()
    {
        stdio_init_all();
        i2c_init(i2c1, 40000);
    }
} _init_inst;

SSD1306 display(2, 3, i2c1);
ScreenManager manager(&display);
Screen screen1(&manager, screen_dimensions);
Screen screen2(&manager, screen_dimensions);
Screen screen3(&manager, screen_dimensions);

TextComponent text1(&manager, Vec2u32{16, 20}, "0", &SSD1306::default_font, 1, &screen1);
TextComponent text2(&manager, Vec2u32{16, 32}, "1", &SSD1306::default_font, 1, &screen1);
TextComponent text3(&manager, Vec2u32{32, 20}, "2", &SSD1306::default_font, 1, &screen1);
TextComponent text4(&manager, Vec2u32{32, 32}, "3", &SSD1306::default_font, 1, &screen1);
PaddingComponent padding(&manager, Vec2u32{0, 16}, {127, 63 - 16}, true, 3, &screen1);
TextBoxComponent hidden_message(&manager, Vec2u32{64, 32}, Vec2u32{64, 32}, Vec2u32{10, 10}, "Message", &SSD1306::default_font, 3, &screen1);

TextBoxComponent screen2_message(&manager, Vec2u32{0, 0}, Vec2u32{127, 63}, Vec2u32{48, 4}, "Screen 2", &SSD1306::default_font, 0, &screen2);

PaddingComponent settings_padding(&manager, Vec2u32{0, 0}, Vec2u32{127, 63}, true, 0, &screen3);
TextComponent settings_back(&manager, Vec2u32{4, 4}, "Back", &SSD1306::default_font, 2, &screen3);
TextBoxComponent settings_display(&manager, Vec2u32{4, 20}, Vec2u32{43, 10}, Vec2u32{2, 2}, "Display", &SSD1306::default_font, 1, &screen3);
TextBoxComponent settings_volume(&manager, Vec2u32{4, 40}, Vec2u32{43, 10}, Vec2u32{2, 2}, "Volume", &SSD1306::default_font, 1, &screen3);

bool lpm = false;
bool screen_pwr = true;
IconComponent battery(&manager, Vec2u32{111, 4}, battery_icon, 0);
TextComponent battery_lpm_message(&manager, Vec2u32{90, 4}, "LPM", &SSD1306::default_font, 0);

CountdownTimer lpm_sleep_timer;

void button_callback_generic(const Event* ev, uint32_t control_mask, const char* name = "")
{
    ButtonEvent* event = (ButtonEvent*)ev;
    if (event->WasPressed())
    {
        if (!screen_pwr)
        {
            display.Power(true);
            screen_pwr = true;
            if (lpm)
                lpm_sleep_timer.Start(5000);

            return;
        }

        manager.QueueControl(control_mask);
        printf("%s Button Pressed.\n", name);

        manager.Update();

        if (lpm)
        {
            lpm_sleep_timer.Start(5000);
        }
    }
}

void select_button_callback(const Event* ev, void*)
{
    button_callback_generic(ev, ControlAction::SELECT0, "Select");
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

int main()
{
    printf("Program start.\n");

    // The initial screen was already given, so this overwrites the LUT entry.
    text1.AddComponentTable(&screen1, &battery, &text2, nullptr, &text3);
    text2.AddComponentTable(&screen1, &text1, nullptr, nullptr, &text4);
    text3.AddComponentTable(&screen1, &battery, &text4, &text1, nullptr);
    text4.AddComponentTable(&screen1, &text3, nullptr, &text2, nullptr);

    // Settings example screen
    settings_back.AddComponentTable(&screen3, nullptr, &settings_display, nullptr, &battery);
    settings_display.AddComponentTable(&screen3, &settings_back, &settings_volume);
    settings_volume.AddComponentTable(&screen3, &settings_display);

    // Since we gave a nullptr as the screen, we must construct a table for each screen using the component.
    battery.AddComponentTable(&screen1, nullptr, &text3);
    battery.AddComponentTable(&screen2);
    battery.AddComponentTable(&screen3, nullptr, &settings_display, &settings_back, nullptr);

    /* // May be used instead for single assignments.
    text1.component_lut[&screen1].down_component = &text2;
    text1.component_lut[&screen1].right_component = &text3;

    text2.component_lut[&screen1].up_component = &text1;
    text2.component_lut[&screen1].right_component = &text4;

    text3.component_lut[&screen1].down_component = &text4;
    text3.component_lut[&screen1].left_component = &text1;

    text4.component_lut[&screen1].up_component = &text3;
    text4.component_lut[&screen1].left_component = &text2;

    text1.component_lut[&screen1].up_component = &battery;
    text3.component_lut[&screen1].up_component = &battery;
    battery.component_lut[&screen1].down_component = &text3;
    */

    hidden_message.SetPersonalVisibility(false); // Hide the message initially.

    battery_lpm_message.SetPersonalVisibility(lpm);

    printf("Created components.\n");

    screen1.AddComponent(&text1);
    screen1.AddComponent(&text2);
    screen1.AddComponent(&text3);
    screen1.AddComponent(&text4);
    screen1.AddComponent(&padding);
    screen1.AddComponent(&hidden_message);
    screen1.AddComponent(&battery);
    screen1.AddComponent(&battery_lpm_message);

    screen2.AddComponent(&screen2_message);
    screen2.AddComponent(&battery);
    screen2.AddComponent(&battery_lpm_message);

    screen3.AddComponent(&settings_padding);
    screen3.AddComponent(&settings_back);
    screen3.AddComponent(&settings_display);
    screen3.AddComponent(&settings_volume);
    screen3.AddComponent(&battery);
    screen3.AddComponent(&battery_lpm_message);

    printf("Added components.\n");

    // Sorts the components by Z-layer. Lowest Z-layer components drawn first.
    // It also chooses the highest Z-layer selectable component by default.
    screen1.SortComponents();
    screen2.SortComponents();
    screen3.SortComponents();

    printf("Sorted components.\n");

    manager.PushScreen(&screen1); // Default screen.

    printf("Pushed screen.\n");

    // Use the event system
    Button left_button = 18;
    Button right_button = 16;
    Button up_button = 17;
    Button down_button = 20;

    Button select_button = 19;
    CountdownTimer hidden_message_timer;

    int id_left = left_button.AddAction(&left_button_callback);
    int id_right = right_button.AddAction(&right_button_callback);
    int id_up = up_button.AddAction(&up_button_callback);
    int id_down = down_button.AddAction(&down_button_callback);
    int id_select = select_button.AddAction(&select_button_callback);

    int id_selecttext1 = text1.AddAction([](const Event* ev, void* ptr){
        hidden_message.SetPersonalVisibility(true);
        CountdownTimer* timer = (CountdownTimer*)ptr;
        timer->Start(3000);
        manager.Update();
    }, &hidden_message_timer);

    int id_hidden_message_timer = hidden_message_timer.AddAction([](const Event*, void*){
        hidden_message.SetPersonalVisibility(false);
        manager.Update();
    });

    CountdownTimer screen2_timer;

    int id_screen2 = screen2_timer.AddAction([](const Event*, void*){
        printf("%d\n", manager.GetScreenCount());
        manager.QueueControl(ControlAction::BACK);
        manager.Update();
        puts("back");
    });

    int id_selecttext2 = text2.AddAction([](const Event*, void* ptr){
        manager.PushScreen(&screen2);
        CountdownTimer* timer = (CountdownTimer*)ptr;
        timer->Start(3000);
    }, &screen2_timer);

    int id_selecttext3 = text3.AddAction([](const Event*, void*){
        manager.PushScreen(&screen3);
    });

    int id_screen3_back = settings_back.AddAction([](const Event*, void*){
        manager.QueueControl(ControlAction::BACK);
        manager.Update();
    });

    int id_battery = battery.AddAction([](const Event*, void*){
        lpm = !lpm;
        battery_lpm_message.SetPersonalVisibility(lpm);
        if (lpm)
            lpm_sleep_timer.Start(5000);
        else
            lpm_sleep_timer.End();

        manager.Update();
    });

    int id_sleep = lpm_sleep_timer.AddAction([](const Event*, void*){
        if (lpm) // failsafe
        {
            display.Power(false);
            screen_pwr = false;
        }
    });

    manager.Update();

    while (1)
    {
        Event::HandleEvents();
    }
}