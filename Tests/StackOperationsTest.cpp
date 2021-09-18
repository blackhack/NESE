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

TEST(StackOperationsTest, TSX) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::TSX);
    cpu.SP = 0xF1;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.SP, cpu.X);
    EXPECT_EQ(cycles, 2);
}

TEST(StackOperationsTest, TXS) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::TXS);
    cpu.X = 0xEC;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.SP, cpu.X);
    EXPECT_EQ(cycles, 2);
}

TEST(StackOperationsTest, PHA_PLA) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::PHA);
    mem[1] = static_cast<uint8_t>(Opcode::PLA);

    cpu.A = 0xFA;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cycles, 3);

    cpu.A = 0x00; // PLA should set it from data in stack

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0xFA);
    EXPECT_EQ(cycles, 4);
}

TEST(StackOperationsTest, PHP_PLP) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::PHP);
    mem[1] = static_cast<uint8_t>(Opcode::PLP);

    cpu.PS.PS_byte = 0b10110101;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cycles, 3);

    cpu.PS.PS_byte = 0b00000000; // PLP should set it from data in stack

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PS.PS_byte, 0b10110101);
    EXPECT_EQ(cycles, 4);
}
