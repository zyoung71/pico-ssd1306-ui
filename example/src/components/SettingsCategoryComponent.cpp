#include <components/SettingsCategoryComponent.h>

std::vector<SettingsCategoryComponent*> SettingsCategoryComponent::category_components;

SettingsCategoryComponent::SettingsCategoryComponent(ScreenManager* manager, const Vec2i32& origin, const Vec2i32& dimensions, const char* text, const Font* font, int32_t z_layer, Screen* initial_screen)
    : TextBoxComponent(manager, origin, dimensions, text, font, z_layer, initial_screen)
{
    category_components.push_back(this);
}
#include <stdio.h>
void SettingsCategoryComponent::OnComponentHovered()
{
    if (origin_position.y + draw_dimensions.max.y > manager->GetCurrentScreen()->GetDimensions().y)
    {
        // move everything up
        for (auto c : category_components)
        {
            c->origin_position -= Vec2i32{0, 20};
        }
    }
    else if (origin_position.y + draw_dimensions.min.y < 16)
    {
        // move everything down
        for (auto c : category_components)
        {
            c->origin_position += Vec2i32{0, 20};
        }
    }
}

void SettingsCategoryComponent::Draw()
{
    TextBoxComponent::Draw();
    // requires this component to have a low z-layer, as this below might obstruct what was drawn before in the area
    display->DrawSquare(Vec2i32{0, 0}, Vec2i32{63, 15}, 0, false);
}