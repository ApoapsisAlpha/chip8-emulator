#include <display.h>

void Display::set_pixel(uint8_t x, uint8_t y, uint8_t state) {
    gfx[width * y + x] = state;
}

uint8_t Display::get_pixel(uint8_t x, uint8_t y) {
    return gfx[width * y + x];
}

uint8_t Display::draw_byte(uint8_t x, uint8_t y, uint8_t byte) {
    uint8_t collision = 0;
    x %= width;
    y %= height;
    for (uint8_t offset = 0; offset < 8; ++offset) {
        uint8_t new_pixel = (byte >> (7 - offset)) & 1;
        uint8_t cur_pixel = get_pixel(x, y);
        if (new_pixel & cur_pixel) {
            collision = 1;
        }

        set_pixel(x, y, new_pixel ^ cur_pixel);
        x = (x + 1) >= width ? 0 : x + 1;
    }

    return collision;
}

void Display::clear() {
    gfx.fill(0);
}