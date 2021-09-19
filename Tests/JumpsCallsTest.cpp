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

TEST(JumpsCallsTest, JMP_ABS) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::JMP_ABS);
    mem[1] = 0xFC;
    mem[2] = 0xFF;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 0xFFFC);
    EXPECT_EQ(cycles, 3);
}

TEST(JumpsCallsTest, JMP_IND) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::JMP_IND);
    mem[1] = 0xED;
    mem[2] = 0x10;
    mem[0x10ED] = 0xFC;
    mem[0x10EE] = 0xFF;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 0xFFFC);
    EXPECT_EQ(cycles, 5);
}

TEST(JumpsCallsTest, JSR_RTS) {
    Memory mem;
    CPU cpu(mem, 1790000);

    mem[0] = static_cast<uint8_t>(Opcode::JSR_ABS);
    mem[1] = 0x21;
    mem[2] = 0xAF;
    mem[0xAF21] = static_cast<uint8_t>(Opcode::RTS);

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 0xAF21);
    EXPECT_EQ(cycles, 6);

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.PC, 3);
    EXPECT_EQ(cycles, 6);
}
