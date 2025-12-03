#include <display/SSD1306.h>
#include <math/Graphics.h>

#include <cstring>

SSD1306::SSD1306(uint8_t sda_pin, uint8_t scl_pin, i2c_inst_t* i2c_inst)
{
    display.external_vcc = false;
    ssd1306_init(&display, 128, 64, 0x3C, i2c_inst);
    ssd1306_clear(&display);
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

void SSD1306::DrawText(Vec2u32 pos, const TextProperties& props, uint32_t)
{
    char vbuff[strlen(props.text) + 1];
    size_t segment_len = 0;
    for (size_t line = 0; line < props.lines; line++)
    {
        size_t next_segment_len = strcspn(props.text + segment_len, "\n");
        strncpy(vbuff, props.text + segment_len, next_segment_len);
        segment_len = next_segment_len + 1;
        
        ssd1306_draw_string_with_font(&display, pos.x, pos.y + line * (props.spacing + props.height), 1, selected_font.data, vbuff);
    }
}
void SSD1306::DrawPixel(Vec2u32 pos, uint32_t color)
{
    if (color == 0)
        ssd1306_clear_pixel(&display, pos.x, pos.y);
    else
        ssd1306_draw_pixel(&display, pos.x, pos.y);
}
void SSD1306::DrawLine(Vec2u32 pos_begin, Vec2u32 pos_end, uint32_t)
{
    ssd1306_draw_line(&display, pos_begin.x, pos_begin.y, pos_end.x, pos_end.y);
}
void SSD1306::DrawPolygon(const Vec2u32* points, size_t pos_count, uint32_t, bool is_outline)
{
    if (is_outline)
    {
        for (size_t i = 0; i < pos_count; i++)
        {
            Vec2u32 begin = points[i];
            Vec2u32 end = points[(i + 1) % pos_count];
            ssd1306_draw_line(&display, begin.x, begin.y, end.x, end.y);
        }
    }
    else
    {
        // fill the polygon
        graphics::scanline_rasterization(points, pos_count, [](Vec2u32 pixel, void* usr){
            ssd1306_draw_pixel((ssd1306_t*)usr, pixel.x, pixel.y);
        }, &display);
    }
}
void SSD1306::DrawSquare(Vec2u32 pos, Vec2u32 size, uint32_t color, bool is_outline)
{
    if (color == 0)
        ssd1306_clear_square(&display, pos.x, pos.y, size.x, size.y);
    else if (is_outline)
        ssd1306_draw_empty_square(&display, pos.x, pos.y, size.x, size.y);
    else
        ssd1306_draw_square(&display, pos.x, pos.y, size.x, size.y);
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