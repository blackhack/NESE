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

TEST(LogicalTest, AND_ZP_X) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::AND_ZP_X);
    mem[1] = 0x80;
    cpu.X = 0x0F;
    mem[0x8F] = 0xE9;
    cpu.A = 0x73;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0xE9 & 0x73);
    EXPECT_EQ(cycles, 4);

    mem[2] = static_cast<uint8_t>(Opcode::AND_ZP_X);
    mem[3] = 0x80;
    cpu.X = 0xFF;
    mem[0x7F] = 0x85;
    cpu.A = 0xD6;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x85 & 0xD6);
    EXPECT_EQ(cycles, 4);
}

TEST(LogicalTest, EOR_ABS_Y) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::EOR_ABS_Y);
    mem[1] = 0xCC;
    mem[2] = 0x1F;
    cpu.Y = 0x01;

    //1FCC + 01 = 1FCD
    mem[0x1FCD] = 0x55;
    cpu.A = 0xD6;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x55 ^ 0xD6);
    EXPECT_EQ(cycles, 4);

    mem[3] = static_cast<uint8_t>(Opcode::EOR_ABS_Y);
    mem[4] = 0xCC;
    mem[5] = 0x1F;
    cpu.Y = 0x34;

    //1FCC + 34 = 2000
    mem[0x2000] = 0x44;
    cpu.A = 0xE7;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x44 ^ 0xE7);
    EXPECT_EQ(cycles, 5);
}

TEST(LogicalTest, ORA_IND_X) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::ORA_IND_X);
    mem[1] = 0x3E;
    mem[0x0043] = 0x15;
    mem[0x0044] = 0x24;
    mem[0x2415] = 0x6E;
    cpu.X = 0x05;
    cpu.A = 0x7A;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x6E | 0x7A);
    EXPECT_EQ(cycles, 6);
}

TEST(LogicalTest, BIT_ABS) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::BIT_ABS);
    mem[1] = 0x15;
    mem[2] = 0x24;
    mem[0x2415] = 0x33; 
    cpu.A = 0xCC;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0xCC);
    EXPECT_TRUE(cpu.PS.flags.Z);
    EXPECT_FALSE(cpu.PS.flags.V);
    EXPECT_FALSE(cpu.PS.flags.N);
    EXPECT_EQ(cycles, 4);

    mem[3] = static_cast<uint8_t>(Opcode::BIT_ABS);
    mem[4] = 0x17;
    mem[5] = 0x35;
    mem[0x3517] = 0x7C;
    cpu.A = 0x4E;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x4E);
    EXPECT_FALSE(cpu.PS.flags.Z);
    EXPECT_TRUE(cpu.PS.flags.V);
    EXPECT_FALSE(cpu.PS.flags.N);
    EXPECT_EQ(cycles, 4);
}
