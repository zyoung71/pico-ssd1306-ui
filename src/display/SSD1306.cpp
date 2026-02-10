#include <display/SSD1306.h>
#include <math/Graphics.h>

#include <cstring>

SSD1306::SSD1306(uint8_t sda_pin, uint8_t scl_pin, i2c_inst_t* i2c_inst, uint8_t addr)
    : sda_pin(sda_pin), scl_pin(scl_pin), addr(addr), i2c_inst(i2c_inst)
{
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    display.external_vcc = false;
    ssd1306_init(&display, 128, 64, addr, i2c_inst);
    ssd1306_clear(&display);
}

SSD1306::SSD1306(const SSD1306& other)
    : sda_pin(sda_pin), scl_pin(scl_pin), addr(other.addr == 0x3C ? 0x3D : 0x3C), i2c_inst(other.i2c_inst)
{
    display.external_vcc = false;
    ssd1306_init(&display, 128, 64, addr, i2c_inst);
    ssd1306_clear(&display);
}

SSD1306& SSD1306::operator=(const SSD1306& other)
{
    if (this == &other)
        return *this;

    display.external_vcc = other.display.external_vcc;

    sda_pin = other.sda_pin;
    scl_pin = other.scl_pin;
    addr = other.addr;
    i2c_inst = other.i2c_inst;

    ssd1306_deinit(&display);
    ssd1306_init(&display, 128, 64, addr, i2c_inst);
    ssd1306_clear(&display);

    return *this;
}

void SSD1306::Power(bool power_on)
{
    if (power_on)
        ssd1306_poweron(&display);
    else
        ssd1306_poweroff(&display);
}

void SSD1306::UpdateDisplay()
{
    ssd1306_show(&display);
}

void SSD1306::DrawText(Vec2i32 pos, const char* text, const Font* font, uint32_t)
{
    uint8_t font_data[font->char_bitmap.length + 5];
    font_data[0] = font->char_height;
    font_data[1] = font->char_width;
    font_data[2] = font->char_spacing;
    font_data[3] = font->ascii_begin;
    font_data[4] = font->ascii_end;
    
    memcpy(font_data + 5, font->char_bitmap.data, font->char_bitmap.length);
    ssd1306_draw_string_with_font(&display, pos.x, pos.y, 1, font_data, text);
}
void SSD1306::DrawPixel(Vec2i32 pos, uint32_t color)
{
    if (color)
        ssd1306_draw_pixel(&display, pos.x, pos.y);
    else
        ssd1306_clear_pixel(&display, pos.x, pos.y);
}
void SSD1306::DrawLine(Vec2i32 pos_begin, Vec2i32 pos_end, uint32_t)
{
    ssd1306_draw_line(&display, pos_begin.x, pos_begin.y, pos_end.x, pos_end.y);
}
void SSD1306::DrawPolygon(const Vec2i32* points, size_t pos_count, uint32_t, bool is_outline)
{
    if (is_outline)
    {
        for (size_t i = 0; i < pos_count; i++)
        {
            Vec2i32 begin = points[i];
            Vec2i32 end = points[(i + 1) % pos_count];
            ssd1306_draw_line(&display, begin.x, begin.y, end.x, end.y);
        }
    }
    else
    {
        // fill the polygon
        graphics::scanline_rasterization(points, pos_count, [](Vec2i32 pixel, void* usr){
            ssd1306_draw_pixel((ssd1306_t*)usr, pixel.x, pixel.y);
        }, &display);
    }
}
void SSD1306::DrawSquare(Vec2i32 pos, Vec2i32 size, uint32_t color, bool is_outline, bool fill_if_outline)
{
    if (color == 0)
        ssd1306_clear_square(&display, pos.x, pos.y, size.x - 1, size.y - 1);
    else if (is_outline)
    {
        ssd1306_draw_empty_square(&display, pos.x, pos.y, size.x - 1, size.y - 1);
        if (fill_if_outline)
            ssd1306_clear_square(&display, pos.x + 1, pos.y + 1, size.x - 2, size.y - 2);
    }
    else
        ssd1306_draw_square(&display, pos.x, pos.y, size.x - 1, size.y - 1);
}

void SSD1306::DrawScrollingText(Vec2i32 pos, const char* text, bool move_left, const Font* font, uint32_t color)
{
    // TODO
}

void SSD1306::DisplayBitmap(const uint8_t* bitmap_buff, size_t bitmap_size)
{
    ssd1306_bmp_show_image(&display, bitmap_buff, bitmap_size);
}

void SSD1306::ClearDisplay()
{
    ssd1306_clear(&display);
}

void SSD1306::InvertColors()
{
    static bool inverted = false;
    inverted = !inverted;
    ssd1306_invert(&display, !inverted);
}

void SSD1306::SetContrast(uint8_t contrast)
{
    ssd1306_contrast(&display, contrast);
}