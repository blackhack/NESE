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


TEST(ArithmeticTest, ADC_ABS) {
    Memory mem;
    CPU cpu(mem);

    cpu.P.Flags.C = 0;
    cpu.A = 127;
    cpu.P.Flags.Z = 1;
    cpu.P.Flags.N = 0;
    cpu.P.Flags.V = 0;
    mem[0] = static_cast<uint8_t>(Opcode::ADC_ABS);
    mem[1] = 0x00;
    mem[2] = 0x80;
    mem[0x8000] = 127;

    uint32_t cycles = cpu.Run(1);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(cpu.A, uint8_t(-2));
    EXPECT_EQ(cpu.P.Flags.C, 0);
    EXPECT_EQ(cpu.P.Flags.Z, 0);
    EXPECT_EQ(cpu.P.Flags.N, 1);
    EXPECT_EQ(cpu.P.Flags.V, 1);
}

TEST(ArithmeticTest, SBC_ABS) {
    Memory mem;
    CPU cpu(mem);

    cpu.P.Flags.C = 1;
    cpu.A = 127;
    cpu.P.Flags.Z = 0;
    cpu.P.Flags.N = 1;
    cpu.P.Flags.V = 1;
    mem[0] = static_cast<uint8_t>(Opcode::SBC_ABS);
    mem[1] = 0x00;
    mem[2] = 0x80;
    mem[0x8000] = 127;

    uint32_t cycles = cpu.Run(1);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(cpu.A, 0);
    EXPECT_EQ(cpu.P.Flags.C, 1);
    EXPECT_EQ(cpu.P.Flags.Z, 1);
    EXPECT_EQ(cpu.P.Flags.N, 0);
    EXPECT_EQ(cpu.P.Flags.V, 0);
}

TEST(ArithmeticTest, CMP_IM) {
    Memory mem;
    CPU cpu(mem);

    cpu.P.Flags.C = 0;
    cpu.A = 127;
    cpu.P.Flags.Z = 0;
    cpu.P.Flags.N = 1;
    mem[0] = static_cast<uint8_t>(Opcode::CMP_IM);
    mem[1] = 127;

    uint32_t cycles = cpu.Run(1);

    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(cpu.A, 127);
    EXPECT_EQ(cpu.P.Flags.C, 1);
    EXPECT_EQ(cpu.P.Flags.Z, 1);
    EXPECT_EQ(cpu.P.Flags.N, 0);
}

TEST(ArithmeticTest, CPX_ZP) {
    Memory mem;
    CPU cpu(mem);

    cpu.P.Flags.C = 1;
    cpu.X = 0;
    cpu.P.Flags.Z = 1;
    cpu.P.Flags.N = 0;
    mem[0] = static_cast<uint8_t>(Opcode::CPX_ZP);
    mem[1] = 0x25;
    mem[0x0025] = 127;

    uint32_t cycles = cpu.Run(1);

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.X, 0);
    EXPECT_EQ(cpu.P.Flags.C, 0);
    EXPECT_EQ(cpu.P.Flags.Z, 0);
    EXPECT_EQ(cpu.P.Flags.N, 1);
}

TEST(ArithmeticTest, CPY_ZP) {
    Memory mem;
    CPU cpu(mem);

    cpu.P.Flags.C = 0;
    cpu.Y = 105;
    cpu.P.Flags.Z = 1;
    cpu.P.Flags.N = 1;
    mem[0] = static_cast<uint8_t>(Opcode::CPY_ZP);
    mem[1] = 0x25;
    mem[0x0025] = 100;

    uint32_t cycles = cpu.Run(1);

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.Y, 105);
    EXPECT_EQ(cpu.P.Flags.C, 1);
    EXPECT_EQ(cpu.P.Flags.Z, 0);
    EXPECT_EQ(cpu.P.Flags.N, 0);
}
