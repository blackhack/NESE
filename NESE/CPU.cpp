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
#include <chrono>
#include <thread>

CPU::CPU(Memory& mem, uint32_t frequency) : memory(mem)
{
    cycle_period_ns = static_cast<uint64_t>((1.0 / frequency) * 1000000000);
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

uint64_t GetNSTime()
{
    static const std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    return uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_time).count());
}

uint32_t CPU::Execute(uint32_t instructions_to_execute)
{
    uint32_t total_cycles = 0;

    while (instructions_to_execute > 0)
    {
        uint64_t start_time = GetNSTime();

        Opcode instruction = static_cast<Opcode>(GetByteFromPC());
        uint8_t instruction_cycles = 0;

        if (opcodesHandlers.find(instruction) == opcodesHandlers.end())
        {
            ++instruction_cycles;
            std::cout << "Unk opcode: " << std::hex << (uint16_t)instruction << "\n";
        }
        else
        {
            OpcodeHandler op_handler = opcodesHandlers.at(instruction);
            instruction_cycles += op_handler.base_cycles;
            instruction_cycles += op_handler.callback(this);
        }

        total_cycles += instruction_cycles;
        --instructions_to_execute;

        uint64_t end_time = GetNSTime();
        uint64_t elapse_instruction_time = end_time - start_time;
        uint64_t expected_instruction_time = cycle_period_ns * instruction_cycles;
        if (elapse_instruction_time < expected_instruction_time)
        {
            //std::cout << "Elapse: " << elapse_instruction_time << " - Expected: " << expected_instruction_time << " - Sleeping for: " << expected_instruction_time - elapse_instruction_time << "\n";
            std::this_thread::sleep_for(std::chrono::nanoseconds(expected_instruction_time - elapse_instruction_time));
        }

        //std::cout << "Final instruction time: " << GetNSTime() - start_time << "ns\n";
    }

    return total_cycles;
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

uint8_t CPU::LDA_IM()
{
    A = GetByteFromPC();
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ZP()
{
    uint16_t address;

    address = GetByteFromPC();
    A = GetByteFromAddress(address);
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ZP_X()
{
    uint16_t ZP_address;

    ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    A = GetByteFromAddress(ZP_address);
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ABS()
{
    uint16_t address;

    address = GetWordFromPC();

    A = GetByteFromAddress(address);
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ABS_X()
{
    uint16_t base_address;
    uint16_t final_address;
    bool page_crossed = false;

    base_address = GetWordFromPC();
    final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = GetByteFromAddress(final_address);
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);


    if (page_crossed)
        return 1;
    else
        return 0;
}


uint8_t CPU::LDA_ABS_Y()
{
    uint16_t base_address;
    uint16_t final_address;
    bool page_crossed = false;

    base_address = GetWordFromPC();

    final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = GetByteFromAddress(final_address);
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    if (page_crossed)
        return 1;
    else
        return 0;
}

uint8_t CPU::LDA_IND_X()
{
    uint16_t ZP_address;
    uint16_t final_address;

    ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    final_address = GetWordFromAddress(ZP_address);

    A = GetByteFromAddress(final_address);
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_IND_Y()
{
    uint16_t ZP_address;
    uint16_t base_address;
    uint16_t final_address;
    bool page_crossed = false;;

    ZP_address = GetByteFromPC();

    base_address = GetWordFromAddress(ZP_address);

    final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = GetByteFromAddress(final_address);
    PS.Z = (A == 0 ? 1 : 0);
    PS.N = checkBit(A, 7);

    if (page_crossed)
        return 1;
    else
        return 0;
}

uint8_t CPU::LDX_IM()
{
    X = GetByteFromPC();
    PS.Z = (X == 0 ? 1 : 0);
    PS.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ZP()
{
    uint16_t address;

    address = GetByteFromPC();

    X = GetByteFromAddress(address);
    PS.Z = (X == 0 ? 1 : 0);
    PS.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ZP_Y()
{
    uint16_t ZP_address;

    ZP_address = GetByteFromPC();

    ZP_address += Y;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    X = GetByteFromAddress(ZP_address);
    PS.Z = (X == 0 ? 1 : 0);
    PS.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ABS()
{
    uint16_t address;

    address = GetWordFromPC();

    X = GetByteFromAddress(address);
    PS.Z = (X == 0 ? 1 : 0);
    PS.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ABS_Y()
{
    uint16_t base_address;
    uint16_t final_address;
    bool page_crossed = false;;

    base_address = GetWordFromPC();

    final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    X = GetByteFromAddress(final_address);
    PS.Z = (X == 0 ? 1 : 0);
    PS.N = checkBit(X, 7);

    if (page_crossed)
        return 1;
    else
        return 0;
}
