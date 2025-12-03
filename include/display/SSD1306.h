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
private:
    uint8_t sda_pin;
    uint8_t scl_pin;

    ssd1306_t display;

    ArrayView<uint8_t> selected_font;

public:
    SSD1306(uint8_t sda_pin, uint8_t scl_pin, i2c_inst_t* i2c_inst = i2c0);

    inline void SetFont(const ArrayView<uint8_t>& font)
    {
        selected_font = font;
    }

    void UpdateDisplay() override;
    void Power(bool power_on) override;

    void DrawText(Vec2u32 pos, const TextProperties& props, uint32_t) override;
    void DrawPixel(Vec2u32 pos, uint32_t);
    void DrawLine(Vec2u32 pos_begin, Vec2u32 pos_end, uint32_t) override;
    void DrawPolygon(const Vec2u32* points, size_t pos_count, uint32_t, bool is_outline = true) override;
    void DrawSquare(Vec2u32 pos, Vec2u32 size, uint32_t, bool is_outline = true) override;

    void DisplayBitmap(const uint8_t* bitmap_buff, size_t bitmap_size) override;
    void ClearDisplay() override;
    void InvertColors() override;
    void SetContrast(uint8_t contrast) override;
};