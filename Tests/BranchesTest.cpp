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

TEST(BranchesTest, BCC_REL) {
    Memory mem;
    CPU cpu(mem);

    cpu.PS.flags.C = 0;
    mem[0] = static_cast<uint8_t>(Opcode::BCC_REL);
    mem[1] = 0x78;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 2+0x78);
    EXPECT_EQ(cycles, 3);
}

TEST(BranchesTest, BVS_REL) {
    Memory mem;
    CPU cpu(mem);

    cpu.PS.flags.V = 1;
    cpu.PC = 0xFD;
    mem[0xFD] = static_cast<uint8_t>(Opcode::BVS_REL);
    mem[0xFE] = 0x78;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 0xFF + 0x78);
    EXPECT_EQ(cycles, 4);
}

TEST(BranchesTest, BNE_REL) {
    Memory mem;
    CPU cpu(mem);

    cpu.PS.flags.N = 0;
    cpu.PC = 0xFD;
    mem[0xFD] = static_cast<uint8_t>(Opcode::BNE_REL);
    mem[0xFE] = 0x88; // This is taken as a signed byte, si 0x88 is -120 or -0x78

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 0xFF - 0x78);
    EXPECT_EQ(cycles, 3);
}
