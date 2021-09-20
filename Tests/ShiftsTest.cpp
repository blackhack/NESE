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

TEST(ShiftsTest, ASL_ACC) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::ASL_ACC);
    cpu.A = 0b10000001;
    cpu.PS.flags.C = 0;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 2);
    EXPECT_EQ(cpu.PS.flags.C, 1);
    EXPECT_EQ(cycles, 2);
}

TEST(ShiftsTest, LSR_ZP) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LSR_ZP);
    mem[1] = 0x15;
    mem[0x0015] = 0b10000001;
    cpu.PS.flags.C = 0;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(mem[0x0015], 64);
    EXPECT_EQ(cpu.PS.flags.C, 1);
    EXPECT_EQ(cycles, 5);
}

TEST(ShiftsTest, ROL_ABS) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::ROL_ABS);
    mem[1] = 0x15;
    mem[2] = 0x78;
    mem[0x7815] = 0b00000001;
    cpu.PS.flags.C = 1;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(mem[0x7815], 0b00000011);
    EXPECT_EQ(cpu.PS.flags.C, 0);
    EXPECT_EQ(cycles, 6);
}

TEST(ShiftsTest, ROR_ABS) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::ROR_ABS);
    mem[1] = 0x15;
    mem[2] = 0x78;
    mem[0x7815] = 0b00000001;
    cpu.PS.flags.C = 1;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(mem[0x7815], 0b10000000);
    EXPECT_EQ(cpu.PS.flags.C, 1);
    EXPECT_EQ(cycles, 6);
}
