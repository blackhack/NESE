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

TEST(RegisterTransferTest, TAX) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::TAX);
    cpu.A = 0x21;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, cpu.X);
    EXPECT_EQ(cycles, 2);
}

TEST(RegisterTransferTest, TAY) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::TAY);
    cpu.Y = 0x22;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, cpu.Y);
    EXPECT_EQ(cycles, 2);
}

TEST(RegisterTransferTest, TXA) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::TXA);
    cpu.X = 0xCF;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, cpu.X);
    EXPECT_EQ(cycles, 2);
}

TEST(RegisterTransferTest, TYA) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::TYA);
    cpu.Y = 0xFF;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, cpu.Y);
    EXPECT_EQ(cycles, 2);
}
