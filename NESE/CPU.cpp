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

#include "CPU.h"
#include <iostream>

CPU::CPU(Memory& mem) : memory(mem)
{
    Reset();
}

void CPU::Reset()
{
    PC = memory[0xFFFC];
    SP = 0;
    A = 0;
    X = 0;
    Y = 0;

    PS.C = 0;
    PS.Z = 0;
    PS.I = 0;
    PS.D = 0;
    PS.B = 0;
    PS.V = 0;
    PS.N = 0;
}

uint32_t CPU::Execute(uint32_t instructions_to_execute)
{
    uint32_t cycles = 0;
    Opcode instruction = Opcode::NULL_OP;
    uint32_t opcode_part = 0;
    bool instruction_finished = true;

    while (instructions_to_execute > 0)
    {
        if (instruction_finished)
        {
            instruction = static_cast<Opcode>(GetByteFromPC());
            opcode_part = 0;
            instruction_finished = false;
            ++cycles;
            continue;
        }

        if (opcodesHandlers.find(instruction) == opcodesHandlers.end())
        {
            instruction_finished = true;
            std::cout << "Unk opcode: " << std::hex << (uint16_t)instruction << "\n";
        }
        else
        {
            OpcodeHandler op_handler = opcodesHandlers.at(instruction);
            instruction_finished = op_handler.callback(this, opcode_part);
        }


        if (instruction_finished)
            --instructions_to_execute;

        ++opcode_part;
        ++cycles;
    }

    return cycles;
}

uint8_t CPU::GetByteFromPC()
{
    uint8_t data = memory[PC];
    ++PC;
    return data;
}

uint16_t CPU::GetWordFromPC()
{
    uint16_t data = memory[PC];
    ++PC;
    data |= static_cast<uint16_t>(memory[PC]) << 8;
    ++PC;

    return data;
}

uint8_t CPU::GetByteFromAddress(uint16_t address)
{
    uint8_t data = memory[address];
    return data;
}

uint16_t CPU::GetWordFromAddress(uint16_t address)
{
    uint16_t data = memory[address];
    data |= static_cast<uint16_t>(memory[address + 1]) << 8;

    return data;
}

bool CPU::LDA_IM(uint32_t& part)
{
    A = GetByteFromPC();
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    return true;
}

bool CPU::LDA_ZP(uint32_t& part)
{
    static uint16_t address;
    
    switch (part)
    {
        case 0:
            address = GetByteFromPC();
            break;
        case 1:
            A = GetByteFromAddress(address);
            PS.Z = (A == 0 ? 1 : 0);
            PS.N = checkBit(A, 7);
            break;
    }

    return part == 1;
}

bool CPU::LDA_ZP_X(uint32_t& part)
{
    static uint16_t ZP_address;

    switch (part)
    {
    case 0:
        ZP_address = GetByteFromPC();
        break;
    case 1:
        ZP_address += X;
        ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)
        break;
    case 2:
        A = GetByteFromAddress(ZP_address);
        PS.Z = (A == 0 ? 1 : 0);
        PS.N = checkBit(A, 7);
        break;
    }

    return part == 2;
}

bool CPU::LDA_ABS(uint32_t& part)
{
    static uint16_t address;

    switch (part)
    {
    case 0:
        address = GetWordFromPC();
        break;
    case 1:
        // Original CPU takes two cycles to fetch the address 2 bytes
        break;
    case 2:
        A = GetByteFromAddress(address);
        PS.Z = (A == 0 ? 1 : 0);
        PS.N = checkBit(A, 7);
        break;
    }

    return part == 2;
}

bool CPU::LDA_ABS_X(uint32_t& part)
{
    static uint16_t base_address;
    static uint16_t final_address;
    static bool page_crossed;

    switch (part)
    {
    case 0:
        base_address = GetWordFromPC();
        page_crossed = false; // Just initializing for now
        break;
    case 1:
        // Original CPU takes two cycles to fetch the address 2 bytes

        final_address = base_address + X;

        if ((final_address ^ base_address) >> 8)
            page_crossed = true;
        break;
    case 2:
        A = GetByteFromAddress(final_address);
        PS.Z = (A == 0 ? 1 : 0);
        PS.N = checkBit(A, 7);
        break;
    case 3:
        // When page is crossed it requires an extra cycle
        break;
    }

    if (page_crossed)
        return part == 3;
    else
        return part == 2;
}


bool CPU::LDA_ABS_Y(uint32_t& part)
{
    static uint16_t base_address;
    static uint16_t final_address;
    static bool page_crossed;

    switch (part)
    {
    case 0:
        base_address = GetWordFromPC();
        page_crossed = false; // Just initializing for now
        break;
    case 1:
        // Original CPU takes two cycles to fetch the address 2 bytes

        final_address = base_address + Y;

        if ((final_address ^ base_address) >> 8)
            page_crossed = true;
        break;
    case 2:
        A = GetByteFromAddress(final_address);
        PS.Z = (A == 0 ? 1 : 0);
        PS.N = checkBit(A, 7);
        break;
    case 3:
        // When page is crossed it requires an extra cycle
        break;
    }

    if (page_crossed)
        return part == 3;
    else
        return part == 2;
}

bool CPU::LDA_IND_X(uint32_t& part)
{
    static uint16_t ZP_address;
    static uint16_t final_address;

    switch (part)
    {
    case 0:
        ZP_address = GetByteFromPC();
        break;
    case 1:
        ZP_address += X;
        ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)
        break;
    case 2:
        final_address = GetWordFromAddress(ZP_address);
        break;
    case 3:
        // Original CPU takes two cycles to fetch the address 2 bytes
        break;
    case 4:
        A = GetByteFromAddress(final_address);
        PS.Z = (A == 0 ? 1 : 0);
        PS.N = checkBit(A, 7);
        break;
    }

    return part == 4;
}

bool CPU::LDA_IND_Y(uint32_t& part)
{
    static uint16_t ZP_address;
    static uint16_t base_address;
    static uint16_t final_address;
    static bool page_crossed;

    switch (part)
    {
    case 0:
        ZP_address = GetByteFromPC();
        page_crossed = false;
        break;
    case 1:
        base_address = GetWordFromAddress(ZP_address);
    case 2:
        // Original CPU takes two cycles to fetch the address 2 bytes

        final_address = base_address + Y;
        if ((final_address ^ base_address) >> 8)
            page_crossed = true;
        break;
    case 3:
        A = GetByteFromAddress(final_address);
        PS.Z = (A == 0 ? 1 : 0);
        PS.N = checkBit(A, 7);
        break;
    case 4:
        break;
    }

    if (page_crossed)
        return part == 4;
    else
        return part == 3;
}

bool CPU::LDX_IM(uint32_t& part)
{
    X = GetByteFromPC();
    PS.Z = (X == 0 ? 1 : 0);
    PS.N = checkBit(X, 7);

    return true;
}

bool CPU::LDX_ZP(uint32_t& part)
{
    static uint16_t address;

    switch (part)
    {
    case 0:
        address = GetByteFromPC();
        break;
    case 1:
        X = GetByteFromAddress(address);
        PS.Z = (X == 0 ? 1 : 0);
        PS.N = checkBit(X, 7);
        break;
    }

    return part == 1;
}

bool CPU::LDX_ZP_Y(uint32_t& part)
{
    static uint16_t ZP_address;

    switch (part)
    {
    case 0:
        ZP_address = GetByteFromPC();
        break;
    case 1:
        ZP_address += Y;
        ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)
        break;
    case 2:
        X = GetByteFromAddress(ZP_address);
        PS.Z = (X == 0 ? 1 : 0);
        PS.N = checkBit(X, 7);
        break;
    }

    return part == 2;
}

bool CPU::LDX_ABS(uint32_t& part)
{
    static uint16_t address;

    switch (part)
    {
    case 0:
        address = GetWordFromPC();
        break;
    case 1:
        // Original CPU takes two cycles to fetch the address 2 bytes
        break;
    case 2:
        X = GetByteFromAddress(address);
        PS.Z = (X == 0 ? 1 : 0);
        PS.N = checkBit(X, 7);
        break;
    }

    return part == 2;
}

bool CPU::LDX_ABS_Y(uint32_t& part)
{
    static uint16_t base_address;
    static uint16_t final_address;
    static bool page_crossed;

    switch (part)
    {
    case 0:
        base_address = GetWordFromPC();
        page_crossed = false; // Just initializing for now
        break;
    case 1:
        // Original CPU takes two cycles to fetch the address 2 bytes

        final_address = base_address + Y;

        if ((final_address ^ base_address) >> 8)
            page_crossed = true;
        break;
    case 2:
        X = GetByteFromAddress(final_address);
        PS.Z = (X == 0 ? 1 : 0);
        PS.N = checkBit(X, 7);
        break;
    case 3:
        // When page is crossed it requires an extra cycle
        break;
    }

    if (page_crossed)
        return part == 3;
    else
        return part == 2;
}
