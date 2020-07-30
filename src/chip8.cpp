#include "chip8.h"

using std::uint8_t;
using std::uint16_t;

Chip8::Chip8() {
    srand(time(0));
    init_opcodes();
    reset();
}

void Chip8::reset() {
    I = 0;
    pc = 0x200;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    display.clear();
    stack.fill(0);
    memory.fill(0);
    std::copy(font_set.begin(), font_set.end(), memory.begin());
}

void Chip8::init_opcode_0_map() {
    // CLS
    opcode_0_map[0x0] = [this]() { display.clear(); };

    // RET
    opcode_0_map[0xE] = [this]() { pc = stack[--sp]; };
}

void Chip8::init_opcode_8_map() {
    // LD Vx, Vy
    opcode_8_map[0x0] = [this](uint8_t x, uint8_t y) { V[x] = V[y]; };

    // OR Vx, Vy
    opcode_8_map[0x1] = [this](uint8_t x, uint8_t y) { V[x] |= V[y]; };

    // AND Vx, Vy
    opcode_8_map[0x2] = [this](uint8_t x, uint8_t y) { V[x] &= V[y]; };

    // XOR Vx, Vy
    opcode_8_map[0x3] = [this](uint8_t x, uint8_t y) { V[x] ^= V[y]; };

    // ADD Vx, Vy
    opcode_8_map[0x4] = [this](uint8_t x, uint8_t y) {
        uint16_t sum = V[x] + V[y];
        V[0xF] = sum > 0xFF;
        V[x] = sum & 0xFF;
    };

    // SUB Vx, Vy
    opcode_8_map[0x5] = [this](uint8_t x, uint8_t y) {
        V[0xF] = V[x] > V[y];
        V[x] -= V[y];
    };

    // SHR Vx
    opcode_8_map[0x6] = [this](uint8_t x, uint8_t y) {
        V[0xF] = V[x] & 1;
        V[x] >>= 1;
    };

    // SUBN Vx, Vy
    opcode_8_map[0x7] = [this](uint8_t x, uint8_t y) {
        V[0xF] = V[y] > V[x];
        V[x] = V[y] - V[x];
    };

    // SHL Vx
    opcode_8_map[0xE] = [this](uint8_t x, uint8_t y) {
        V[0xF] = V[x] & 1;
        V[x] <<= 1;
    };
}

void Chip8::init_opcode_e_map() {
    // SKP Vx
    opcode_e_map[0xE] = [this](uint8_t x) {
        if (keypad[V[x]]) {
            pc += 2;
        }
    };

    // SKNP Vx
    opcode_e_map[0x1] = [this](uint8_t x) {
        if (!keypad[V[x]]) {
            pc += 2;
        }
    };
}

void Chip8::init_opcode_f_map() {
    // LD Vx, DT
    opcode_f_map[0x07] = [this](uint8_t x) { V[x] = delay_timer; };

    // LD Vx, K
    opcode_f_map[0x0A] = [this](uint8_t x) {
        pc -= 2;
        for (uint8_t key = 0; key < (uint8_t) keypad.size(); ++key) {
            if (keypad[key]) {
                V[x] = key;
                pc += 2;
                break;
            }
        }
    };

    // LD DT, Vx
    opcode_f_map[0x15] = [this](uint8_t x) { delay_timer = V[x]; };

    // LD ST, Vx
    opcode_f_map[0x18] = [this](uint8_t x) { sound_timer = V[x]; };

    // ADD I, Vx
    opcode_f_map[0x1E] = [this](uint8_t x) { I += V[x]; };

    // LD F, Vx
    opcode_f_map[0x1E] = [this](uint8_t x) { I = V[x] * 5; };

    // LD B, Vx
    opcode_f_map[0x1E] = [this](uint8_t x) {
        memory[I] = V[x] / 100;
        memory[I + 1] = (V[x] / 10) % 10;
        memory[I + 2] = V[x] % 10;
    };

    // LD [I], Vx
    opcode_f_map[0x1E] = [this](uint8_t x) {
        for (uint8_t i = 0; i <= x; ++i) {
            memory[I + i] = V[i];
        }
    };

    // LD Vx, [I]
    opcode_f_map[0x1E] = [this](uint8_t x) {
        for (uint8_t i = 0; i <= x; ++i) {
            V[i] = memory[I + i];
        }
    };
}

void Chip8::init_opcode_table() {
    // Refer to 0x0 submap
    opcode_table[0x0] = [this](uint16_t ins_data) { opcode_0_map[ins_data & 0xF](); };

    // JP addr
    opcode_table[0x1] = [this](uint16_t ins_data) { pc = ins_data; };

    // CALL addr
    opcode_table[0x2] = [this](uint16_t ins_data) {
        stack[sp++] = pc;
        pc = ins_data;
    };

    // SE Vx, byte
    opcode_table[0x3] = [this](uint16_t ins_data) {
        if (V[ins_data >> 8] == ins_data & 0xFF) {
            pc += 2;
        }
    };

    // SNE Vx, byte
    opcode_table[0x4] = [this](uint16_t ins_data) {
        if (V[ins_data >> 8] != ins_data & 0xFF) {
            pc += 2;
        }
    };

    // SE Vx, Vy
    opcode_table[0x5] = [this](uint16_t ins_data) {
        if (V[ins_data >> 8] == V[(ins_data & 0xF0) >> 4]) {
            pc += 2;
            }
    };
    
    // LD Vx, byte
    opcode_table[0x6] = [this](uint16_t ins_data) { V[ins_data >> 8] = ins_data & 0xFF; };

    // ADD Vx, byte
    opcode_table[0x7] = [this](uint16_t ins_data) { V[ins_data >> 8] += ins_data & 0xFF; };

    // Refer to 0x8 submap
    opcode_table[0x8] = [this](uint16_t ins_data) { opcode_8_map[ins_data & 0xF](ins_data >> 8, (ins_data & 0xF0) >> 4); };

    // SNE Vx, Vy
    opcode_table[0x9] = [this](uint16_t ins_data) {
        if (V[ins_data >> 8] != V[(ins_data & 0xF0) >> 4]) {
            pc += 2;
        }
    };

    // LD I, addr
    opcode_table[0xA] = [this](uint16_t ins_data) { I = ins_data; };

    // JP V0, addr
    opcode_table[0xB] = [this](uint16_t ins_data) { pc = ins_data + V[0]; };

    // RND Vx, byte
    opcode_table[0xC] = [this](uint16_t ins_data) { V[ins_data >> 8] = (ins_data & 0xFF) & (rand() % 256); };

    // DRW Vx, Vy, nibble
    opcode_table[0xD] = [this](uint16_t ins_data) {
        // Initial location for drawing the sprite
        uint8_t x = V[ins_data >> 8];
        uint8_t y = V[(ins_data & 0xF0) >> 4];

        V[0xF] = 0;
        for (uint8_t offset = 0; offset < (ins_data & 0xF); ++offset) {
            uint16_t addr = I + offset;
            uint8_t collision = display.draw_byte(x, y, addr);
            if (collision) {
                V[0xF] = 1;
            }

            y = (y + 1) >= display.height ? 0 : y + 1;
        }
    };

    // Refer to 0xE submap
    opcode_table[0xE] = [this](uint16_t ins_data) { opcode_e_map[ins_data & 0xF](ins_data >> 8); };

    // Refer to 0xF submap
    opcode_table[0xE] = [this](uint16_t ins_data) { opcode_f_map[ins_data & 0xFF](ins_data >> 8); };
}

void Chip8::init_opcodes() {
    init_opcode_0_map();
    init_opcode_8_map();
    init_opcode_e_map();
    init_opcode_f_map();
    init_opcode_table();
}

void Chip8::process_cycle() {
    // Read current opcode
    opcode = memory[pc << 8] | memory[pc + 1];
    pc += 2;

    // Execute the instruction
    opcode_table[opcode >> 12](opcode & 0x0FFF);
}

void Chip8::update_timers() {
    if (sound_timer > 0) {
        --sound_timer;
    }

    if (delay_timer > 0) {
        --delay_timer;
    }
}