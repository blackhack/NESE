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

#include "Opcode.h"
#include "CPU.h"

OpcodeHandler::OpcodeHandler() : name(""), callback(nullptr)
{
}

OpcodeHandler::OpcodeHandler(std::string opName, uint8_t opBase_cycles, OpcodeCallback opCallback)
{
    name = opName;
    base_cycles = opBase_cycles;
    callback = opCallback;
}

#define DEFINE_HANDLER(opcode, base_cycles, callback) { Opcode::opcode, OpcodeHandler(#opcode, base_cycles, callback) }

std::map<Opcode, OpcodeHandler> opcodesHandlers = {
    DEFINE_HANDLER(LDA_IM, 2, &CPU::LDA_IM),
    DEFINE_HANDLER(LDA_ZP, 3, &CPU::LDA_ZP),
    DEFINE_HANDLER(LDA_ZP_X, 4, &CPU::LDA_ZP_X),
    DEFINE_HANDLER(LDA_ABS, 4, &CPU::LDA_ABS),
    DEFINE_HANDLER(LDA_ABS_X, 4, &CPU::LDA_ABS_X),
    DEFINE_HANDLER(LDA_ABS_Y, 4, &CPU::LDA_ABS_Y),
    DEFINE_HANDLER(LDA_IND_X, 6, &CPU::LDA_IND_X),
    DEFINE_HANDLER(LDA_IND_Y, 5, &CPU::LDA_IND_Y),
    DEFINE_HANDLER(LDX_IM, 2, &CPU::LDX_IM),
    DEFINE_HANDLER(LDX_ZP, 3, &CPU::LDX_ZP),
    DEFINE_HANDLER(LDX_ZP_Y, 4, &CPU::LDX_ZP_Y),
    DEFINE_HANDLER(LDX_ABS, 4, &CPU::LDX_ABS),
    DEFINE_HANDLER(LDX_ABS_Y, 4, &CPU::LDX_ABS_Y),
};
