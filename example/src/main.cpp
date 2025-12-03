#include <display/SSD1306.h>
#include <interactive-ui/ScreenManager.h>
#include <interactive-ui/components/TextComponent.h>

constexpr Vec2u32 screen_dimensions = {128, 64};

int main()
{
    SSD1306 display(4, 5);
    display.SetFont(make_array_view(font_8x5));

    ScreenManager manager(&display);

    Screen screen(&manager, screen_dimensions);

    TextComponent text(&screen, Vec2u32{64, 32}, TextProperties{"TEXT", 8, 5, 1, 1}, 0);    

    screen.AddComponent(&text);

    screen.SortComponents();

    manager.PushScreen(&screen);

    while (1)
    {
        manager.Update();
    }
}