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

#include <iostream>
#include "CPU.h"
#include "Memory.h"

int main()
{
    Memory ram;
    CPU cpu(ram, 1790000);

    ram[0] = static_cast<uint8_t>(Opcode::LDA_IM); // Opcode::LDA_IMM
    ram[1] = 0x31; // Random value for the opcode
    ram[2] = static_cast<uint8_t>(Opcode::LDA_ZP); // Opcode::LDA_ZP
    ram[3] = 0xF3; // Random address for the opcode
    ram[0xF3] = 0x84; // Random value for the opcode on the random address

    cpu.Reset();

    cpu.Execute(1);
    std::cout << std::hex << (int)cpu.A << std::endl;
    cpu.Execute(1);
    std::cout << std::hex << (int)cpu.A << std::endl;

    return 0;
}
