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

TEST(LoadStoreTest, LDA_FlagsTest) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_IM);
    mem[1] = 0x00; // Random value for the opcode
    mem[2] = static_cast<uint8_t>(Opcode::LDA_IM);
    mem[3] = 0x0F; // Random value for the opcode
    mem[4] = static_cast<uint8_t>(Opcode::LDA_IM);
    mem[5] = 0xFF; // Random value for the opcode

    cpu.Execute(1);
    EXPECT_EQ(cpu.P.Flags.Z, 1);
    EXPECT_EQ(cpu.P.Flags.N, 0);
    cpu.Execute(1);
    EXPECT_EQ(cpu.P.Flags.Z, 0);
    EXPECT_EQ(cpu.P.Flags.N, 0);
    cpu.Execute(1);
    EXPECT_EQ(cpu.P.Flags.Z, 0);
    EXPECT_EQ(cpu.P.Flags.N, 1);

}

TEST(LoadStoreTest, LDA_IM) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_IM);
    mem[1] = 0x31; // Random value for the opcode

    uint32_t cycles = cpu.Execute(1);

    EXPECT_EQ(cpu.A, 0x31);
    EXPECT_EQ(cycles, 2);
}

TEST(LoadStoreTest, LDA_ZP) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_ZP);
    mem[1] = 0xF3; // Random address for the opcode
    mem[0xF3] = 0x84; // Random value for the opcode on the random address

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(cycles, 3);
}

TEST(LoadStoreTest, LDA_ZP_X) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_ZP_X);
    mem[1] = 0x80; // Random address for the opcode
    cpu.X = 0x0F; // Random value for X, so it takes the address as 0x80 + 0x0F = 0x8F
    mem[0x8F] = 0x84; // Random value for the opcode on the random address

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(cycles, 4);

    mem[2] = static_cast<uint8_t>(Opcode::LDA_ZP_X);
    mem[3] = 0x80; // Random address for the opcode
    cpu.X = 0xFF; // Random value for X, so it takes the address as 0x80 + 0xFF = 0x017F > 0x7F truncation
    mem[0x7F] = 0x85; // Random value for the opcode on the random address

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x85);
    EXPECT_EQ(cycles, 4);
}

TEST(LoadStoreTest, LDA_ABS) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_ABS);
    mem[1] = 0x2F;
    mem[2] = 0xFF;
    mem[0xFF2F] = 0x55;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x55);
    EXPECT_EQ(cycles, 4);
}

TEST(LoadStoreTest, LDA_ABS_X) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_ABS_X);
    mem[1] = 0xCC;
    mem[2] = 0x1F;
    cpu.X = 0x01;

    //1FCC + 01 = 1FCD
    mem[0x1FCD] = 0x55;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x55);
    EXPECT_EQ(cycles, 4);

    mem[3] = static_cast<uint8_t>(Opcode::LDA_ABS_X);
    mem[4] = 0xCC;
    mem[5] = 0x1F;
    cpu.X = 0x34;

    //1FCC + 34 = 2000
    mem[0x2000] = 0x44;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x44);
    EXPECT_EQ(cycles, 5);
}

TEST(LoadStoreTest, LDA_ABS_Y) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_ABS_Y);
    mem[1] = 0xCC;
    mem[2] = 0x1F;
    cpu.Y = 0x01;

    //1FCC + 01 = 1FCD
    mem[0x1FCD] = 0x55;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x55);
    EXPECT_EQ(cycles, 4);

    mem[3] = static_cast<uint8_t>(Opcode::LDA_ABS_Y);
    mem[4] = 0xCC;
    mem[5] = 0x1F;
    cpu.Y = 0x34;

    //1FCC + 34 = 2000
    mem[0x2000] = 0x44;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x44);
    EXPECT_EQ(cycles, 5);
}

TEST(LoadStoreTest, LDA_IND_X) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_IND_X);
    mem[1] = 0x3E;
    mem[0x0043] = 0x15;
    mem[0x0044] = 0x24;
    mem[0x2415] = 0x6E;
    cpu.X = 0x05;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x6E);
    EXPECT_EQ(cycles, 6);
}

TEST(LoadStoreTest, LDA_IND_Y) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDA_IND_Y);
    mem[1] = 0x4C;
    mem[0x4C] = 0x00;
    mem[0x4D] = 0x21;
    mem[0x2105] = 0x6D;
    cpu.Y = 0x05;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0x6D);
    EXPECT_EQ(cycles, 5);

    mem[2] = static_cast<uint8_t>(Opcode::LDA_IND_Y);
    mem[3] = 0x4C;
    mem[0x4C] = 0x21;
    mem[0x4D] = 0x21;
    mem[0x2212] = 0xDD;
    cpu.Y = 0xF1;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, 0xDD);
    EXPECT_EQ(cycles, 6);
}

TEST(LoadStoreTest, LDX_ZP_Y) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDX_ZP_Y);
    mem[1] = 0x80;
    cpu.Y = 0x0F;
    mem[0x8F] = 0x84;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.X, 0x84);
    EXPECT_EQ(cycles, 4);

    mem[2] = static_cast<uint8_t>(Opcode::LDX_ZP_Y);
    mem[3] = 0x80;
    cpu.Y = 0xFF;
    mem[0x7F] = 0x85;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.X, 0x85);
    EXPECT_EQ(cycles, 4);
}

TEST(LoadStoreTest, LDY_ABS_X) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::LDY_ABS_X);
    mem[1] = 0xCC;
    mem[2] = 0x1F;
    cpu.X = 0x01;

    //1FCC + 01 = 1FCD
    mem[0x1FCD] = 0x55;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.Y, 0x55);
    EXPECT_EQ(cycles, 4);

    mem[3] = static_cast<uint8_t>(Opcode::LDY_ABS_X);
    mem[4] = 0xCC;
    mem[5] = 0x1F;
    cpu.X = 0x34;

    //1FCC + 34 = 2000
    mem[0x2000] = 0x44;

    cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.Y, 0x44);
    EXPECT_EQ(cycles, 5);
}

TEST(LoadStoreTest, STA_IND_Y) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::STA_IND_Y);
    mem[1] = 0x4C;
    mem[0x4C] = 0x00;
    mem[0x4D] = 0x21;
    cpu.Y = 0x05;
    cpu.A = 0xCD;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.A, mem[0x2105]);
    EXPECT_EQ(cycles, 6);
}

TEST(LoadStoreTest, STX_ZP) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::STX_ZP);
    mem[1] = 0xF3;
    cpu.X = 0x84;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.X, mem[0xF3]);
    EXPECT_EQ(cycles, 3);
}

TEST(LoadStoreTest, STY_ABS) {
    Memory mem;
    CPU cpu(mem);

    mem[0] = static_cast<uint8_t>(Opcode::STY_ABS);
    mem[1] = 0x2F;
    mem[2] = 0xFF;
    cpu.Y = 0x55;

    uint32_t cycles = cpu.Execute(1);
    EXPECT_EQ(cpu.Y, mem[0xFF2F]);
    EXPECT_EQ(cycles, 4);
}
