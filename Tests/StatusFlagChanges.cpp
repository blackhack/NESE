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
#include "Bus.h"

TEST(StatusFlagChanges, CLC) {
    Bus mem;
    CPU cpu(mem);

    cpu.P.Flags.C = 1;
    mem[0] = static_cast<uint8_t>(Opcode::CLC);

    uint32_t cycles = cpu.Run(1);
    EXPECT_EQ(cpu.P.Flags.C, 0);
    EXPECT_EQ(cycles, 2);
}

TEST(StatusFlagChanges, SEC) {
    Bus mem;
    CPU cpu(mem);

    cpu.P.Flags.C = 0;
    mem[0] = static_cast<uint8_t>(Opcode::SEC);

    uint32_t cycles = cpu.Run(1);
    EXPECT_EQ(cpu.P.Flags.C, 1);
    EXPECT_EQ(cycles, 2);
}
