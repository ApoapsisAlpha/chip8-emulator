#pragma once
#include <cstdint>
#include <array>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <display.h>

class Chip8 {
    public:
        Chip8();
        void reset();
        void process_cycle();
        void update_timers();

        // Current opcode (each instruction takes up a total of 2 bytes)
        uint16_t opcode;

        // The chip's memory
        std::array<uint8_t, 4096> memory;

        /**
        The 16 8-bit registers where V0 to VE are used for general purpose.
        Meanwhile, VF is a specialized register which is used in the following
        ways:
        - In an addition operation, VF is the carry flag
        - In a subtraction operation, VF is the no borrow flag
        - In the draw instruction, VF is set during pixel collision
        */
        std::array<uint8_t, 16> V;

        // The 16 bit address register, I, and program counter, pc
        uint16_t I;
        uint16_t pc;

        /**
        Two timers that count down at 60 Hz.
        The delay timer is intended to be used for timing game events.
        The sound timer is used for sound effects which occur when its is
        nonzero
        */
        uint8_t delay_timer;
        uint8_t sound_timer;

        // The state of the display (64 x 32)
        Display display;

        // The state of the keypad
        std::array<uint8_t, 16> keypad;

        // Stack and stack pointer
        std::array<uint8_t, 16> stack;
        uint8_t sp;
    
    private:
        void init_opcodes();
        void init_opcode_table();
        void init_opcode_0_map();
        void init_opcode_8_map();
        void init_opcode_e_map();
        void init_opcode_f_map();

        std::array<std::function<void(uint16_t ins_data)>, 16> opcode_table;
        std::unordered_map<uint8_t, std::function<void()>> opcode_0_map;
        std::unordered_map<uint8_t, std::function<void(uint8_t x, uint8_t y)>> opcode_8_map;
        std::unordered_map<uint8_t, std::function<void(uint8_t x)>> opcode_e_map;
        std::unordered_map<uint8_t, std::function<void(uint8_t x)>> opcode_f_map;

        std::array<uint8_t, 80> font_set = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80, // F
        }
};