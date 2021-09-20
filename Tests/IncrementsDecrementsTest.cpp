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

TEST(IncrementsDecrementsTest, INC_ZP_X) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::INC_ZP_X);
    mem[1] = 0x80;
    cpu.X = 0x0F;
    mem[0x8F] = 100;

    mem[2] = static_cast<uint8_t>(Opcode::INC_ZP_X);
    mem[3] = 0x80;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(mem[0x8F], 101);
    EXPECT_EQ(cycles, 6);

    cycles = cpu.Execute(1);
    EXPECT_EQ(mem[0x8F], 102);
    EXPECT_EQ(cycles, 6);
}

TEST(IncrementsDecrementsTest, DEC_ABS_X) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::DEC_ABS_X);
    mem[1] = 0xCC;
    mem[2] = 0x1F;
    cpu.X = 0x01;

    //1FCC + 01 = 1FCD
    mem[0x1FCD] = 100;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(mem[0x1FCD], 99);
    EXPECT_EQ(cycles, 7);

    mem[3] = static_cast<uint8_t>(Opcode::DEC_ABS_X);
    mem[4] = 0xCC;
    mem[5] = 0x1F;

    cycles = cpu.Execute(1);
    EXPECT_EQ(mem[0x1FCD], 98);
    EXPECT_EQ(cycles, 7);
}

TEST(IncrementsDecrementsTest, INX_DEX) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::INX);
    mem[1] = static_cast<uint8_t>(Opcode::DEX);
    cpu.X = 100;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.X, 101);
    EXPECT_EQ(cycles, 2);

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.X, 100);
    EXPECT_EQ(cycles, 2);
}

TEST(IncrementsDecrementsTest, INY_DEY) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::INY);
    mem[1] = static_cast<uint8_t>(Opcode::DEY);
    cpu.Y = 255;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.Y, 0);
    EXPECT_EQ(cycles, 2);

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.Y, 255);
    EXPECT_EQ(cycles, 2);
}
