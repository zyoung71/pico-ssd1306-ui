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
    constexpr static Font default_font = Font::BuildFont<8, 5, 1>(font_8x5 + 5);

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

    void UpdateDisplay() override;
    void Power(bool power_on) override;

    void DrawText(Vec2i32 pos, const char* text, const Font* font, uint32_t) override;
    void DrawPixel(Vec2i32 pos, uint32_t);
    void DrawLine(Vec2i32 pos_begin, Vec2i32 pos_end, uint32_t) override;
    void DrawPolygon(const Vec2i32* points, size_t pos_count, uint32_t, bool is_outline = true) override;
    void DrawSquare(Vec2i32 pos, Vec2i32 size, uint32_t, bool is_outline = true, bool fill_if_outline = false) override;

    void DrawScrollingText(Vec2i32 pos, const char* text, bool move_left, const Font* font, uint32_t color) override;

    void DisplayBitmap(const uint8_t* bitmap_buff, size_t bitmap_size) override;
    void ClearDisplay() override;
    void InvertColors() override;
    void SetContrast(uint8_t contrast) override;
};