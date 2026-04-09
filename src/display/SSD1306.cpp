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

void SSD1306::SetBrightness(uint8_t brightness)
{
    ssd1306_contrast(&display, brightness);
}

void SSD1306::DrawPixel(Vec2i32 pos, RGBA color)
{
    if (color.alpha > 0)
        ssd1306_draw_pixel(&display, pos.x, pos.y);
    else
        ssd1306_clear_pixel(&display, pos.x, pos.y);
}

void SSD1306::DrawPixel(int32_t x, int32_t y, RGBA color)
{
    if (color.alpha > 0)
        ssd1306_draw_pixel(&display, x, y);
    else
        ssd1306_clear_pixel(&display, x, y);
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