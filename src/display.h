#pragma once
#include <cstdint>
#include <array>

using std::uint8_t;

class Display {
    public:
        /**
        Draw the given byte starting at location (x, y).
        If any part of the byte is drawn outside of the coordinates of
        the display, it will wrap around to the other side of the display.
        @param data - the byte to draw
        @return 1 if there is a collision and 0 otherwise
        */
        uint8_t draw_byte(uint8_t x, uint8_t y, uint8_t byte);

        /**
        Clear the display
        */
        void clear();

        // The width and height of the display
        static constexpr uint8_t width = 64;
        static constexpr uint8_t height = 32;

        // The pixel states of the display
        std::array<uint8_t, width * height> gfx;
    
    private:
        /**
        Set the pixel at location (x, y) to the given state.
        @param x - the x coordinate of the location
        @param y - the y coordinate of the location
        @param state - the state of the pixel
        */
        void set_pixel(uint8_t x, uint8_t y, uint8_t state);

        /**
        Get the state of the pixel at location (x, y).
        @param x - the x coordinate of the location
        @param y - the y coordinate of the location
        @return the state of the pixel at location (x, y)
        */
        uint8_t get_pixel(uint8_t x, uint8_t y);
};