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

OpcodeHandler::OpcodeHandler(const std::string opName, OpcodeCallback opCallback) : name(opName), callback(opCallback)
{

}

#define DEFINE_HANDLER(opcode, callback) { Opcode::opcode, OpcodeHandler(#opcode, callback) }

std::map<Opcode, OpcodeHandler> opcodesHandlers = {
    DEFINE_HANDLER(LDA_IM, &CPU::LDA_IM),
    DEFINE_HANDLER(LDA_ZP, &CPU::LDA_ZP),
    DEFINE_HANDLER(LDA_ZP_X, &CPU::LDA_ZP_X),
    DEFINE_HANDLER(LDA_ABS, &CPU::LDA_ABS),
    DEFINE_HANDLER(LDA_ABS_X, &CPU::LDA_ABS_X),
    DEFINE_HANDLER(LDA_ABS_Y, &CPU::LDA_ABS_Y),
    DEFINE_HANDLER(LDA_IND_X, &CPU::LDA_IND_X),
    DEFINE_HANDLER(LDA_IND_Y, &CPU::LDA_IND_Y),
    DEFINE_HANDLER(LDX_IM, &CPU::LDX_IM),
    DEFINE_HANDLER(LDX_ZP, &CPU::LDX_ZP),
    DEFINE_HANDLER(LDX_ZP_Y, &CPU::LDX_ZP_Y),
    DEFINE_HANDLER(LDX_ABS, &CPU::LDX_ABS),
    DEFINE_HANDLER(LDX_ABS_Y, &CPU::LDX_ABS_Y),
};
