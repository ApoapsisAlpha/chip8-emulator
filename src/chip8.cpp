#include "chip8.h"

void Chip8::init_opcodes() {
    opcode_table[0x0] = [this](uint16_t ins_data) { opcode_0_map[ins_data & 0xF](); };
    opcode_table[0x1] = [this](uint16_t ins_data) { pc = ins_data; };
    opcode_table[0x2] = [this](uint16_t ins_data) { stack[sp++] = pc; pc = ins_data; };
    opcode_table[0x3] = [this](uint16_t ins_data) { if (V[ins_data >> 8] == ins_data & 0xFF) pc += 2; };
    opcode_table[0x4] = [this](uint16_t ins_data) { if (V[ins_data >> 8] != ins_data & 0xFF) pc += 2; };
    opcode_table[0x5] = [this](uint16_t ins_data) { if (V[ins_data >> 8] == V[(ins_data & 0xF0) >> 4]) pc += 2; };
    opcode_table[0x6] = [this](uint16_t ins_data) { V[ins_data >> 8] = ins_data & 0xFF; };
    opcode_table[0x7] = [this](uint16_t ins_data) { V[ins_data >> 8] += ins_data & 0xFF; };
    opcode_table[0x8] = [this](uint16_t ins_data) { opcode_8_map[ins_data & 0xF](ins_data >> 8, (ins_data & 0xF0) >> 4); };
    opcode_table[0x9] = [this](uint16_t ins_data) { if (V[ins_data >> 8] != V[(ins_data & 0xF0) >> 4]) pc += 2; };
    opcode_table[0xA] = [this](uint16_t ins_data) { I = ins_data; };
    opcode_table[0xB] = [this](uint16_t ins_data) { pc = ins_data + V[0]; };
    opcode_table[0xC] = [this](uint16_t ins_data) { V[ins_data >> 8] = rand() % 256; };
}
