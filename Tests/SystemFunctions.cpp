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

#include <gtest/gtest.h>
#include "CPU.h"
#include "Memory.h"

TEST(SystemFunctions, BRK_RTI) {
    Memory mem;
    CPU cpu(mem);

    cpu.P.Pbyte = 0b11000001;

    mem[0] = static_cast<uint8_t>(Opcode::BRK);
    mem[0xFFFE] = 0x25;
    mem[0xFFFF] = 0xAE;
    mem[0xAE25] = static_cast<uint8_t>(Opcode::RTI);

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 0xAE25);
    EXPECT_EQ(cycles, 7);

    cpu.P.Pbyte = 0b00110100;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 2);
    EXPECT_EQ(cpu.P.Pbyte, 0b11000001);
    EXPECT_EQ(cycles, 6);
}

TEST(SystemFunctions, NOP) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::NOP);
    mem[1] = static_cast<uint8_t>(Opcode::NOP);
    mem[2] = static_cast<uint8_t>(Opcode::NOP);
    mem[3] = static_cast<uint8_t>(Opcode::NOP);
    mem[4] = static_cast<uint8_t>(Opcode::NOP);
    mem[5] = static_cast<uint8_t>(Opcode::NOP);
    mem[6] = static_cast<uint8_t>(Opcode::NOP);
    mem[7] = static_cast<uint8_t>(Opcode::NOP);
    mem[8] = static_cast<uint8_t>(Opcode::NOP);

    uint32_t cycles = cpu.Execute(9);

    EXPECT_EQ(cycles, 18);
}
