#pragma once

#include <interactive-ui/DisplayInterface.h>

#include <util/ArrayView.h>

extern "C"
{
#include <ssd1306.h>
#include <font.h>
}

class SSD1306 : public DisplayInterface
{
public:
    constexpr static Font default_font = Font::BuildFont<8, 5, 1>(font_8x5 + 5); // take original font data only

private:
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint8_t addr;

    i2c_inst_t* i2c_inst;

    ssd1306_t display;

public:
    SSD1306(uint8_t sda_pin, uint8_t scl_pin, i2c_inst_t* i2c_inst = i2c0, uint8_t addr = 0x3C);
    SSD1306(const SSD1306& other);
    SSD1306& operator=(const SSD1306& other);

    inline constexpr Vec2u32 GetDimensions() const override
    {
        return {128, 64};
    }

    void UpdateDisplay() override;
    void Power(bool power_on) override;
    void ClearDisplay() override;
    void InvertColors() override;
    void SetBrightness(uint8_t brightness) override;

    void DrawPixel(Vec2i32 pos, RGBA color) override;
    void DrawPixel(int32_t x, int32_t y, RGBA color) override;
};