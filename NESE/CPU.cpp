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
    PC = GetWordFromAddress(0xFFFC);
    SP = 0xFF;
    A = 0;
    X = 0;
    Y = 0;

    P.Flags.C = 0;
    P.Flags.Z = 0;
    P.Flags.I = 0;
    P.Flags.D = 0;
    P.Flags.B = 0;
    P.Flags.V = 0;
    P.Flags.N = 0;
}

uint32_t CPU::Execute(uint32_t instructions_to_execute)
{
    uint32_t total_cycles = 0;

    while (instructions_to_execute > 0)
    {
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

void CPU::SetByte(uint16_t address, uint8_t data)
{
    memory[address] = data;
}

void CPU::SetWord(uint16_t address, uint16_t data)
{
    memory[address] = data & 0xFF;
    memory[address + 1] = data >> 8;
}

void CPU::PushByteToStack(uint8_t data)
{
    memory[0x0100 + SP] = data;
    --SP;
}

void CPU::PushWordToStack(uint16_t data)
{
    memory[0x0100 + SP] = data >> 8;
    --SP;
    memory[0x0100 + SP] = data & 0xFF;
    --SP;
}

uint8_t CPU::PullByteFromStack()
{
    ++SP;
    uint8_t data = memory[0x0100 + SP];

    return data;
}

uint16_t CPU::PullWordFromStack()
{
    ++SP;
    uint16_t data = memory[0x0100 + SP];
    ++SP;
    data |= static_cast<uint16_t>(memory[0x0100 + SP]) << 8;

    return data;
}

uint8_t CPU::LDA_IM()
{
    A = GetByteFromPC();
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    A = GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    A = GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ABS()
{
    uint16_t address = GetWordFromPC();

    A = GetByteFromAddress(address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);


    return (page_crossed ? 1 : 0);
}


uint8_t CPU::LDA_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::LDA_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);

    A = GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LDA_IND_Y()
{
    bool page_crossed = false;

    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);

    uint16_t final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::LDX_IM()
{
    X = GetByteFromPC();
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ZP()
{
    uint16_t ZP_address = GetByteFromPC();

    X = GetByteFromAddress(ZP_address);
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ZP_Y()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += Y;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    X = GetByteFromAddress(ZP_address);
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ABS()
{
    uint16_t address = GetWordFromPC();

    X = GetByteFromAddress(address);
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::LDX_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    X = GetByteFromAddress(final_address);
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::LDY_IM()
{
    Y = GetByteFromPC();
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return 0;
}

uint8_t CPU::LDY_ZP()
{
    uint16_t ZP_address = GetByteFromPC();

    Y = GetByteFromAddress(ZP_address);
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return 0;
}

uint8_t CPU::LDY_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    Y = GetByteFromAddress(ZP_address);
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return 0;
}

uint8_t CPU::LDY_ABS()
{
    uint16_t address = GetWordFromPC();

    Y = GetByteFromAddress(address);
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return 0;
}

uint8_t CPU::LDY_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    Y = GetByteFromAddress(final_address);
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::STA_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    SetByte(ZP_address, A);

    return 0;
}

uint8_t CPU::STA_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    SetByte(ZP_address, A);

    return 0;
}

uint8_t CPU::STA_ABS()
{
    uint16_t address = GetWordFromPC();
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STA_ABS_X()
{
    uint16_t address = GetWordFromPC() + X;
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STA_ABS_Y()
{
    uint16_t address = GetWordFromPC() + Y;
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STA_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);
    SetByte(final_address, A);

    return 0;
}

uint8_t CPU::STA_IND_Y()
{
    uint16_t ZP_address = GetByteFromPC();
    uint16_t final_address = GetWordFromAddress(ZP_address) + Y;
    SetByte(final_address, A);

    return 0;
}

uint8_t CPU::STX_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    SetByte(ZP_address, X);

    return 0;
}

uint8_t CPU::STX_ZP_Y()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += Y;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    SetByte(ZP_address, X);

    return 0;
}

uint8_t CPU::STX_ABS()
{
    uint16_t address = GetWordFromPC();
    SetByte(address, X);

    return 0;
}

uint8_t CPU::STY_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    SetByte(ZP_address, Y);

    return 0;
}

uint8_t CPU::STY_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    SetByte(ZP_address, Y);

    return 0;
}

uint8_t CPU::STY_ABS()
{
    uint16_t address = GetWordFromPC();
    SetByte(address, Y);

    return 0;
}

uint8_t CPU::TAX()
{
    X = A;
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::TAY()
{
    Y = A;
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return 0;
}

uint8_t CPU::TXA()
{
    A = X;
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::TYA()
{
    A = Y;
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::TSX()
{
    X = SP;
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::TXS()
{
    SP = X;

    return 0;
}

uint8_t CPU::PHA()
{
    PushByteToStack(A);

    return 0;
}

uint8_t CPU::PHP()
{
    P.Flags.B = 1;
    P.Flags.U = 1;
    PushByteToStack(P.Pbyte);

    return 0;
}

uint8_t CPU::PLA()
{
    A = PullByteFromStack();
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::PLP()
{
    P.Pbyte = PullByteFromStack();
    P.Flags.B = 0;
    P.Flags.U = 0;

    return 0;
}

uint8_t CPU::AND_IM()
{
    A &= GetByteFromPC();
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::AND_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    A &= GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::AND_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    A &= GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::AND_ABS()
{
    uint16_t address = GetWordFromPC();

    A &= GetByteFromAddress(address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::AND_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A &= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::AND_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A &= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::AND_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);

    A &= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::AND_IND_Y()
{
    bool page_crossed = false;

    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);

    uint16_t final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A &= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::EOR_IM()
{
    A ^= GetByteFromPC();
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::EOR_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    A ^= GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::EOR_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    A ^= GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::EOR_ABS()
{
    uint16_t address = GetWordFromPC();

    A ^= GetByteFromAddress(address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::EOR_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A ^= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::EOR_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A ^= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::EOR_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);

    A ^= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::EOR_IND_Y()
{
    bool page_crossed = false;

    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);

    uint16_t final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A ^= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::ORA_IM()
{
    A |= GetByteFromPC();
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ORA_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    A |= GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ORA_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    A |= GetByteFromAddress(ZP_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ORA_ABS()
{
    uint16_t address = GetWordFromPC();

    A |= GetByteFromAddress(address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ORA_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A |= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::ORA_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A |= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::ORA_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);

    A |= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ORA_IND_Y()
{
    bool page_crossed = false;

    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);

    uint16_t final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A |= GetByteFromAddress(final_address);
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::BIT_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);
    uint8_t result = A & data;
    P.Flags.Z = (result == 0 ? 1 : 0);
    P.Flags.V = checkBit(data, 6);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::BIT_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);
    uint8_t result = A & data;

    P.Flags.Z = (result == 0 ? 1 : 0);
    P.Flags.V = checkBit(data, 6);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ADC(uint8_t a, uint8_t b)
{
    uint16_t total = a + b + P.Flags.C;
    uint8_t result = total & 0xFF;
    bool same_sign_before = checkBit(~(a ^ b), 7);
    bool same_sign_after = checkBit(~(result ^ b), 7);

    P.Flags.C = total > 0xFF;
    P.Flags.V = (same_sign_before && !same_sign_after);

    return result;
}

uint8_t CPU::SBC(uint8_t a, uint8_t b)
{
    return ADC(a, ~b);
}

uint8_t CPU::ADC_IM()
{
    A = ADC(A, GetByteFromPC());

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ADC_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    A = ADC(A, GetByteFromAddress(ZP_address));

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ADC_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    A = ADC(A, GetByteFromAddress(ZP_address));

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ADC_ABS()
{
    uint16_t address = GetWordFromPC();
    A = ADC(A, GetByteFromAddress(address));

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ADC_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = ADC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::ADC_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = ADC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::ADC_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);

    A = ADC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ADC_IND_Y()
{
    bool page_crossed = false;

    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);

    uint16_t final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = ADC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::SBC_IM()
{
    A = SBC(A, GetByteFromPC());

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::SBC_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    A = SBC(A, GetByteFromAddress(ZP_address));

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::SBC_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    A = SBC(A, GetByteFromAddress(ZP_address));

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::SBC_ABS()
{
    uint16_t address = GetWordFromPC();
    A = SBC(A, GetByteFromAddress(address));

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::SBC_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = SBC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::SBC_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = SBC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::SBC_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);

    A = SBC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::SBC_IND_Y()
{
    bool page_crossed = false;

    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);

    uint16_t final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    A = SBC(A, GetByteFromAddress(final_address));
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::CMP_IM()
{
    uint8_t data = GetByteFromPC();

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return 0;
}

uint8_t CPU::CMP_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return 0;
}

uint8_t CPU::CMP_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;// This address require a truncation to 8 bits (zero page address)

    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return 0;
}

uint8_t CPU::CMP_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return 0;
}

uint8_t CPU::CMP_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address);
    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::CMP_ABS_Y()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address);
    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::CMP_IND_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    uint16_t final_address = GetWordFromAddress(ZP_address);

    uint8_t data = GetByteFromAddress(final_address);
    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return 0;
}

uint8_t CPU::CMP_IND_Y()
{
    bool page_crossed = false;

    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);

    uint16_t final_address = base_address + Y;
    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address);
    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::CPX_IM()
{
    uint8_t data = GetByteFromPC();

    P.Flags.C = (X >= data ? 1 : 0);
    P.Flags.Z = (X== data ? 1 : 0);
    P.Flags.N = checkBit((X - data), 7);

    return 0;
}

uint8_t CPU::CPX_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = (X >= data ? 1 : 0);
    P.Flags.Z = (X == data ? 1 : 0);
    P.Flags.N = checkBit((X - data), 7);

    return 0;
}

uint8_t CPU::CPX_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);

    P.Flags.C = (X >= data ? 1 : 0);
    P.Flags.Z = (X == data ? 1 : 0);
    P.Flags.N = checkBit((X - data), 7);

    return 0;
}

uint8_t CPU::CPY_IM()
{
    uint8_t data = GetByteFromPC();

    P.Flags.C = (Y >= data ? 1 : 0);
    P.Flags.Z = (Y == data ? 1 : 0);
    P.Flags.N = checkBit((Y - data), 7);

    return 0;
}

uint8_t CPU::CPY_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = (Y >= data ? 1 : 0);
    P.Flags.Z = (Y == data ? 1 : 0);
    P.Flags.N = checkBit((Y - data), 7);

    return 0;
}

uint8_t CPU::CPY_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);

    P.Flags.C = (Y >= data ? 1 : 0);
    P.Flags.Z = (Y == data ? 1 : 0);
    P.Flags.N = checkBit((Y - data), 7);

    return 0;
}

uint8_t CPU::INC_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address) + 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::INC_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;

    uint8_t data = GetByteFromAddress(ZP_address) + 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::INC_ABS()
{
    uint16_t address = GetWordFromPC();

    uint8_t data = GetByteFromAddress(address) + 1;
    SetByte(address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::INC_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address) + 1;
    SetByte(final_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::INX()
{
    ++X;
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::INY()
{
    ++Y;
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return 0;
}

uint8_t CPU::DEC_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address) - 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::DEC_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;

    uint8_t data = GetByteFromAddress(ZP_address) - 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::DEC_ABS()
{
    uint16_t address = GetWordFromPC();

    uint8_t data = GetByteFromAddress(address) - 1;
    SetByte(address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::DEC_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address) - 1;
    SetByte(final_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::DEX()
{
    --X;
    P.Flags.Z = (X == 0 ? 1 : 0);
    P.Flags.N = checkBit(X, 7);

    return 0;
}

uint8_t CPU::DEY()
{
    --Y;
    P.Flags.Z = (Y == 0 ? 1 : 0);
    P.Flags.N = checkBit(Y, 7);

    return 0;
}

uint8_t CPU::ASL_ACC()
{
    P.Flags.C = checkBit(A, 7);
    A <<= 1;
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ASL_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ASL_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;

    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ASL_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ASL_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address);

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(final_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::LSR_ACC()
{
    P.Flags.C = checkBit(A, 0);
    A >>= 1;
    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::LSR_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::LSR_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;

    uint8_t data = GetByteFromAddress(ZP_address);

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::LSR_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::LSR_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address);

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(final_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::ROL_ACC()
{
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(A, 7);
    A <<= 1;
    if (old_carry)
        setBit(A, 0);

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ROL_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);

    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ROL_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;

    uint8_t data = GetByteFromAddress(ZP_address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);

    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ROL_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);

    SetByte(address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ROL_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);

    SetByte(final_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::ROR_ACC()
{
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(A, 0);
    A >>= 1;
    if (old_carry)
        setBit(A, 7);

    P.Flags.Z = (A == 0 ? 1 : 0);
    P.Flags.N = checkBit(A, 7);

    return 0;
}

uint8_t CPU::ROR_ZP()
{
    uint16_t ZP_address = GetByteFromPC();
    uint8_t data = GetByteFromAddress(ZP_address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);

    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ROR_ZP_X()
{
    uint16_t ZP_address = GetByteFromPC();

    ZP_address += X;
    ZP_address &= 0xFF;

    uint8_t data = GetByteFromAddress(ZP_address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);

    SetByte(ZP_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ROR_ABS()
{
    uint16_t address = GetWordFromPC();
    uint8_t data = GetByteFromAddress(address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);

    SetByte(address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ROR_ABS_X()
{
    bool page_crossed = false;

    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if ((final_address ^ base_address) >> 8)
        page_crossed = true;

    uint8_t data = GetByteFromAddress(final_address);
    uint8_t old_carry = P.Flags.C;

    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);

    SetByte(final_address, data);

    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);

    return (page_crossed ? 1 : 0);
}

uint8_t CPU::JMP_ABS()
{
    uint16_t address = GetWordFromPC();
    PC = address;

    return 0;
}

uint8_t CPU::JMP_IND()
{
    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = GetWordFromAddress(base_address);
    PC = final_address;

    return 0;
}

uint8_t CPU::JSR_ABS()
{
    uint16_t address = GetWordFromPC();
    PushWordToStack(PC - 1);
    PC = address;

    return 0;
}

uint8_t CPU::RTS()
{
    PC = PullWordFromStack() + 1;

    return 0;
}

uint8_t CPU::BCC_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.C == 0)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::BCS_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.C == 1)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::BEQ_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.Z == 1)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::BMI_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.N == 1)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::BNE_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.Z == 0)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::BPL_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.N == 0)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::BVC_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.V == 0)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::BVS_REL()
{
    int8_t displacement = static_cast<int8_t>(GetByteFromPC());
    uint8_t extra_cycles = 0;

    if (P.Flags.V == 1)
    {
        uint16_t old_PC = PC;
        PC += displacement;
        ++extra_cycles;

        if ((PC ^ old_PC) >> 8)
            ++extra_cycles;
    }

    return extra_cycles;
}

uint8_t CPU::CLC()
{
    P.Flags.C = 0;

    return 0;
}

uint8_t CPU::CLD()
{
    P.Flags.D = 0;

    return 0;
}

uint8_t CPU::CLI()
{
    P.Flags.I = 0;

    return 0;
}

uint8_t CPU::CLV()
{
    P.Flags.V = 0;

    return 0;
}

uint8_t CPU::SEC()
{
    P.Flags.C = 1;

    return 0;
}

uint8_t CPU::SED()
{
    P.Flags.D = 1;

    return 0;
}

uint8_t CPU::SEI()
{
    P.Flags.I = 1;

    return 0;
}

uint8_t CPU::BRK()
{
    // Apparently BRK is in reality a two byte instruction
    // but this byte just get discarded.
    // We need to advance PC anyway.
    ++PC;

    P.Flags.B = 1;
    P.Flags.U = 1;

    PushWordToStack(PC);
    PushByteToStack(P.Pbyte);
    PC = GetWordFromAddress(0xFFFE);

    P.Flags.I = 1;

    return 0;
}

uint8_t CPU::NOP()
{
    return 0;
}

uint8_t CPU::RTI()
{
    P.Pbyte = PullByteFromStack();
    PC = PullWordFromStack();

    P.Flags.B = 0;
    P.Flags.U = 0;

    return 0;
}
