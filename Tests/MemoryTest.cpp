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

TEST(MemoryTest, BasicAssignment) {
    Bus mem;
    for (int i = 0; i < MAX_MEMORY; ++i)
        EXPECT_EQ(mem[i], 0);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_IM);
    mem[1] = 0x31;
    mem[2] = 123;
    mem[0xF3] = 0x84;

    EXPECT_EQ(mem[0], static_cast<uint8_t>(Opcode::LDA_IM));
    EXPECT_EQ(mem[1], 0x31);
    EXPECT_EQ(mem[2], 123);
    EXPECT_EQ(mem[0xF3], 0x84);
}
