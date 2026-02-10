#pragma once

#include <interactive-ui/components/TextComponent.h>

class SettingsCategoryComponent : public TextBoxComponent
{
private:
    static std::vector<SettingsCategoryComponent*> category_components;

public:
    SettingsCategoryComponent(ScreenManager* manager, const Vec2i32& origin, const Vec2i32& dimensions, const char* text, const Font* font, int32_t z_layer, Screen* initial_screen = nullptr);

    void OnComponentHovered() override;
    void Draw() override;
};