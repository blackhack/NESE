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
};

typedef std::function<bool(CPU*, uint32_t&)> OpcodeCallback;

struct OpcodeHandler
{
    OpcodeHandler();
    OpcodeHandler(std::string opName, OpcodeCallback opCallback);

    std::string name;
    OpcodeCallback callback;
};

extern std::map<Opcode, OpcodeHandler> opcodesHandlers;

#endif // Opcode_h__
