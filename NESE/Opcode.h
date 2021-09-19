/*
    NES - MOS 6502 Emulator
    Copyright (C) 2021 JDavid(Blackhack) <davidaristi.0504@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef Opcode_h__
#define Opcode_h__

#include <cstdint>
#include <iostream>
#include <functional>
#include <map>

class CPU;

enum class Opcode : uint8_t
{
    NULL_OP = 0x00,
    LDA_IM = 0xA9,
    LDA_ZP = 0xA5,
    LDA_ZP_X = 0xB5,
    LDA_ABS = 0xAD,
    LDA_ABS_X = 0xBD,
    LDA_ABS_Y = 0xB9,
    LDA_IND_X = 0xA1,
    LDA_IND_Y = 0xB1,

    LDX_IM = 0xA2,
    LDX_ZP = 0xA6,
    LDX_ZP_Y = 0xB6,
    LDX_ABS = 0xAE,
    LDX_ABS_Y = 0xBE,

    LDY_IM = 0xA0,
    LDY_ZP = 0xA4,
    LDY_ZP_X = 0xB4,
    LDY_ABS = 0xAC,
    LDY_ABS_X = 0xBC,

    STA_ZP = 0x82,
    STA_ZP_X = 0x95,
    STA_ABS = 0x8D,
    STA_ABS_X = 0x9D,
    STA_ABS_Y = 0x99,
    STA_IND_X = 0x81,
    STA_IND_Y = 0x91,

    STX_ZP = 0x86,
    STX_ZP_Y = 0x96,
    STX_ABS = 0x8E,

    STY_ZP = 0x84,
    STY_ZP_X = 0x94,
    STY_ABS = 0x8C,

    TAX = 0xAA,
    TAY = 0xA8,
    TXA = 0x8A,
    TYA = 0x98,

    TSX = 0xBA,
    TXS = 0x9A,
    PHA = 0x48,
    PHP = 0x08,
    PLA = 0x68,
    PLP = 0x28,

    AND_IM = 0x29,
    AND_ZP = 0x25,
    AND_ZP_X = 0x35,
    AND_ABS = 0x2D,
    AND_ABS_X = 0x3D,
    AND_ABS_Y = 0x39,
    AND_IND_X = 0x21,
    AND_IND_Y = 0x31,

    EOR_IM = 0x49,
    EOR_ZP = 0x45,
    EOR_ZP_X = 0x55,
    EOR_ABS = 0x4D,
    EOR_ABS_X = 0x5D,
    EOR_ABS_Y = 0x59,
    EOR_IND_X = 0x41,
    EOR_IND_Y = 0x51,

    ORA_IM = 0x09,
    ORA_ZP = 0x05,
    ORA_ZP_X = 0x15,
    ORA_ABS = 0x0D,
    ORA_ABS_X = 0x1D,
    ORA_ABS_Y = 0x19,
    ORA_IND_X = 0x01,
    ORA_IND_Y = 0x11,

    BIT_ZP = 0x24,
    BIT_ABS = 0x2C,

    ADC_IM = 0x69,
    ADC_ZP = 0x65,
    ADC_ZP_X = 0x75,
    ADC_ABS = 0x6D,
    ADC_ABS_X = 0x7D,
    ADC_ABS_Y = 0x79,
    ADC_IND_X = 0x61,
    ADC_IND_Y = 0x71,

    SBC_IM = 0xE9,
    SBC_ZP = 0xE5,
    SBC_ZP_X = 0xF5,
    SBC_ABS = 0xED,
    SBC_ABS_X = 0xFD,
    SBC_ABS_Y = 0xF9,
    SBC_IND_X = 0xE1,
    SBC_IND_Y = 0xF1,

    CMP_IM = 0xC9,
    CMP_ZP = 0xC5,
    CMP_ZP_X = 0xD5,
    CMP_ABS = 0xCD,
    CMP_ABS_X = 0xDD,
    CMP_ABS_Y = 0xD9,
    CMP_IND_X = 0xC1,
    CMP_IND_Y = 0xD1,

    CPX_IM = 0xE0,
    CPX_ZP = 0xE4,
    CPX_ABS = 0xEC,

    CPY_IM = 0xC0,
    CPY_ZP = 0xC4,
    CPY_ABS = 0xCC,

    INC_ZP = 0xE6,
    INC_ZP_X = 0xF6,
    INC_ABS = 0xEE,
    INC_ABS_X = 0xFE,

    INX = 0xE8,
    INY = 0xC8,

    DEC_ZP = 0xC6,
    DEC_ZP_X = 0xD6,
    DEC_ABS = 0xCE,
    DEC_ABS_X = 0xDE,

    DEX = 0xCA,
    DEY = 0x88,

    ASL_ACC = 0x0A,
    ASL_ZP = 0x06,
    ASL_ZP_X = 0x16,
    ASL_ABS = 0x0E,
    ASL_ABS_X = 0x1E,

    LSR_ACC = 0x4A,
    LSR_ZP = 0x46,
    LSR_ZP_X = 0x56,
    LSR_ABS = 0x4E,
    LSR_ABS_X = 0x5E,

    ROL_ACC = 0x2A,
    ROL_ZP = 0x26,
    ROL_ZP_X = 0x36,
    ROL_ABS = 0x2E,
    ROL_ABS_X = 0x3E,

    ROR_ACC = 0x6A,
    ROR_ZP = 0x66,
    ROR_ZP_X = 0x76,
    ROR_ABS = 0x6E,
    ROR_ABS_X = 0x7E,

    JMP_ABS = 0x4C,
    JMP_IND = 0x6C,

    JSR_ABS = 0x20,

    RTS = 0x60,

    BCC_REL = 0x90,
    BCS_REL = 0xB0,
    BEQ_REL = 0xF0,
    BMI_REL = 0x30,
    BNE_REL = 0xD0,
    BPL_REL = 0x10,
    BVC_REL = 0x50,
    BVS_REL = 0x70,
};

typedef std::function<uint8_t(CPU*)> OpcodeCallback;

struct OpcodeHandler
{
    OpcodeHandler();
    OpcodeHandler(std::string opName, uint8_t opBase_cycles, OpcodeCallback opCallback);

    std::string name;
    uint8_t base_cycles;
    OpcodeCallback callback;
};

extern std::map<Opcode, OpcodeHandler> opcodesHandlers;

#endif // Opcode_h__
