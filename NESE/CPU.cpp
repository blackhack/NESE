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

CPU::CPU(Bus& mem) : memory(mem)
{
    IRQ_pending = false;
    NMI_pending = false;
    RESET_pending = false;
    RESET();
}

uint32_t CPU::Run(uint32_t instructions_to_execute)
{
    uint32_t total_cycles = 0;

    while (instructions_to_execute > 0)
    {
        uint8_t instruction_cycles = 0;

        // First handle any pending external interruption
        if (IRQ_pending)
            instruction_cycles += IRQ();
        else if (NMI_pending)
            instruction_cycles += NMI();
        else if (RESET_pending)
            instruction_cycles += RESET();
        else // Normal CPU execution
        {
            Opcode instruction = static_cast<Opcode>(GetByteFromPC());
            
            if (opcodesHandlers.find(instruction) == opcodesHandlers.end())
                instruction_cycles += NOT_IMPLEMENTED(static_cast<uint16_t>(instruction));
            else
            {
                OpcodeHandler op_handler = opcodesHandlers.at(instruction);
                instruction_cycles += op_handler.base_cycles;
                instruction_cycles += op_handler.callback(this);
            }
        }

        IRQ_pending = false;
        NMI_pending = false;
        RESET_pending = false;

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
    memory[STACK_VECTOR + SP] = data;
    --SP;
}

void CPU::PushWordToStack(uint16_t data)
{
    memory[STACK_VECTOR + SP] = data >> 8;
    --SP;
    memory[STACK_VECTOR + SP] = data & 0xFF;
    --SP;
}

uint8_t CPU::PullByteFromStack()
{
    ++SP;
    uint8_t data = memory[STACK_VECTOR + SP];

    return data;
}

uint16_t CPU::PullWordFromStack()
{
    ++SP;
    uint16_t data = memory[STACK_VECTOR + SP];
    ++SP;
    data |= static_cast<uint16_t>(memory[STACK_VECTOR + SP]) << 8;

    return data;
}

uint8_t CPU::GetDataImmediate(uint8_t *extra_cycles, uint16_t * obtained_address)
{
    if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = PC;

    return GetByteFromPC();
}

uint8_t CPU::GetDataZeroPage(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t ZP_address = GetByteFromPC();

    if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = ZP_address;

    return GetByteFromAddress(ZP_address);
}

uint8_t CPU::GetDataZeroPageX(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t ZP_address = GetByteFromPC();
    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = ZP_address;

    return GetByteFromAddress(ZP_address);
}

uint8_t CPU::GetDataZeroPageY(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t ZP_address = GetByteFromPC();
    ZP_address += Y;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)

    if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = ZP_address;

    return GetByteFromAddress(ZP_address);
}

uint8_t CPU::GetDataAbsolute(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t address = GetWordFromPC();

    if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = address;

    return GetByteFromAddress(address);
}

uint8_t CPU::GetDataAbsoluteX(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + X;

    if (extra_cycles && (final_address ^ base_address) >> 8)
        *extra_cycles = 1;
    else if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = final_address;

    return GetByteFromAddress(final_address);
}

uint8_t CPU::GetDataAbsoluteY(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t base_address = GetWordFromPC();
    uint16_t final_address = base_address + Y;

    if (extra_cycles && (final_address ^ base_address) >> 8)
        *extra_cycles = 1;
    else if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = final_address;

    return GetByteFromAddress(final_address);
}

uint8_t CPU::GetDataIndirectX(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t ZP_address = GetByteFromPC();
    ZP_address += X;
    ZP_address &= 0xFF; // This address require a truncation to 8 bits (zero page address)
    uint16_t final_address = GetWordFromAddress(ZP_address);

    if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = final_address;

    return GetByteFromAddress(final_address);
}

uint8_t CPU::GetDataIndirectY(uint8_t* extra_cycles, uint16_t* obtained_address)
{
    uint16_t ZP_address = GetByteFromPC();
    uint16_t base_address = GetWordFromAddress(ZP_address);
    uint16_t final_address = base_address + Y;

    if (extra_cycles && (final_address ^ base_address) >> 8)
        *extra_cycles = 1;
    else if (extra_cycles)
        *extra_cycles = 0;
    if (obtained_address)
        *obtained_address = final_address;

    return GetByteFromAddress(final_address);
}

void CPU::SetNegativeAndZeroFlags(uint8_t data)
{
    P.Flags.Z = (data == 0 ? 1 : 0);
    P.Flags.N = checkBit(data, 7);
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

uint8_t CPU::NOT_IMPLEMENTED(uint16_t instruction)
{
    std::cout << "NOT_IMPLEMENTED: " << std::hex << instruction << " - At: " << PC - 1 << "\n";
    return 1;
}

uint8_t CPU::LDA_IM()
{
    uint8_t extra_cycles = 0;
    A = GetDataImmediate(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDA_ZP()
{
    uint8_t extra_cycles = 0;
    A = GetDataZeroPage(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDA_ZP_X()
{
    uint8_t extra_cycles = 0;
    A = GetDataZeroPageX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDA_ABS()
{
    uint8_t extra_cycles = 0;
    A = GetDataAbsolute(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDA_ABS_X()
{
    uint8_t extra_cycles = 0;
    A = GetDataAbsoluteX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDA_ABS_Y()
{
    uint8_t extra_cycles = 0;
    A = GetDataAbsoluteY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDA_IND_X()
{
    uint8_t extra_cycles = 0;
    A = GetDataIndirectX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDA_IND_Y()
{
    uint8_t extra_cycles = 0;
    A = GetDataIndirectY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::LDX_IM()
{
    uint8_t extra_cycles = 0;
    X = GetDataImmediate(&extra_cycles);
    SetNegativeAndZeroFlags(X);

    return extra_cycles;
}

uint8_t CPU::LDX_ZP()
{
    uint8_t extra_cycles = 0;
    X = GetDataZeroPage(&extra_cycles);
    SetNegativeAndZeroFlags(X);

    return extra_cycles;
}

uint8_t CPU::LDX_ZP_Y()
{
    uint8_t extra_cycles = 0;
    X = GetDataZeroPageY(&extra_cycles);
    SetNegativeAndZeroFlags(X);

    return extra_cycles;
}

uint8_t CPU::LDX_ABS()
{
    uint8_t extra_cycles = 0;
    X = GetDataAbsolute(&extra_cycles);
    SetNegativeAndZeroFlags(X);

    return extra_cycles;
}

uint8_t CPU::LDX_ABS_Y()
{
    uint8_t extra_cycles = 0;
    X = GetDataAbsoluteY(&extra_cycles);
    SetNegativeAndZeroFlags(X);

    return extra_cycles;
}

uint8_t CPU::LDY_IM()
{
    uint8_t extra_cycles = 0;
    Y = GetDataImmediate(&extra_cycles);
    SetNegativeAndZeroFlags(Y);

    return extra_cycles;
}

uint8_t CPU::LDY_ZP()
{
    uint8_t extra_cycles = 0;
    Y = GetDataZeroPage(&extra_cycles);
    SetNegativeAndZeroFlags(Y);

    return extra_cycles;
}

uint8_t CPU::LDY_ZP_X()
{
    uint8_t extra_cycles = 0;
    Y = GetDataZeroPageX(&extra_cycles);
    SetNegativeAndZeroFlags(Y);

    return extra_cycles;
}

uint8_t CPU::LDY_ABS()
{
    uint8_t extra_cycles = 0;
    Y = GetDataAbsolute(&extra_cycles);
    SetNegativeAndZeroFlags(Y);

    return extra_cycles;
}

uint8_t CPU::LDY_ABS_X()
{
    uint8_t extra_cycles = 0;
    Y = GetDataAbsoluteX(&extra_cycles);
    SetNegativeAndZeroFlags(Y);

    return extra_cycles;
}

uint8_t CPU::STA_ZP()
{
    uint16_t ZP_address;
    GetDataZeroPage(nullptr, &ZP_address);
    SetByte(ZP_address, A);

    return 0;
}

uint8_t CPU::STA_ZP_X()
{
    uint16_t ZP_address;
    GetDataZeroPageX(nullptr, &ZP_address);
    SetByte(ZP_address, A);

    return 0;
}

uint8_t CPU::STA_ABS()
{
    uint16_t address;
    GetDataAbsolute(nullptr, &address);
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STA_ABS_X()
{
    uint16_t address;
    GetDataAbsoluteX(nullptr, &address);
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STA_ABS_Y()
{
    uint16_t address;
    GetDataAbsoluteY(nullptr, &address);
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STA_IND_X()
{
    uint16_t address;
    GetDataIndirectX(nullptr, &address);
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STA_IND_Y()
{
    uint16_t address;
    GetDataIndirectY(nullptr, &address);
    SetByte(address, A);

    return 0;
}

uint8_t CPU::STX_ZP()
{
    uint16_t ZP_address;
    GetDataZeroPage(nullptr, &ZP_address);
    SetByte(ZP_address, X);

    return 0;
}

uint8_t CPU::STX_ZP_Y()
{
    uint16_t ZP_address;
    GetDataZeroPageY(nullptr, &ZP_address);
    SetByte(ZP_address, X);

    return 0;
}

uint8_t CPU::STX_ABS()
{
    uint16_t address;
    GetDataAbsolute(nullptr, &address);
    SetByte(address, X);

    return 0;
}

uint8_t CPU::STY_ZP()
{
    uint16_t ZP_address;
    GetDataZeroPage(nullptr, &ZP_address);
    SetByte(ZP_address, Y);

    return 0;
}

uint8_t CPU::STY_ZP_X()
{
    uint16_t ZP_address;
    GetDataZeroPageX(nullptr, &ZP_address);
    SetByte(ZP_address, Y);

    return 0;
}

uint8_t CPU::STY_ABS()
{
    uint16_t address;
    GetDataAbsolute(nullptr, &address);
    SetByte(address, Y);

    return 0;
}

uint8_t CPU::TAX()
{
    X = A;
    SetNegativeAndZeroFlags(X);

    return 0;
}

uint8_t CPU::TAY()
{
    Y = A;
    SetNegativeAndZeroFlags(Y);

    return 0;
}

uint8_t CPU::TXA()
{
    A = X;
    SetNegativeAndZeroFlags(A);

    return 0;
}

uint8_t CPU::TYA()
{
    A = Y;
    SetNegativeAndZeroFlags(A);

    return 0;
}

uint8_t CPU::TSX()
{
    X = SP;
    SetNegativeAndZeroFlags(X);

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
    SetNegativeAndZeroFlags(A);

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
    uint8_t extra_cycles = 0;
    A &= GetDataImmediate(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::AND_ZP()
{
    uint8_t extra_cycles = 0;
    A &= GetDataZeroPage(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::AND_ZP_X()
{
    uint8_t extra_cycles = 0;
    A &= GetDataZeroPageX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::AND_ABS()
{
    uint8_t extra_cycles = 0;
    A &= GetDataAbsolute(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::AND_ABS_X()
{
    uint8_t extra_cycles = 0;
    A &= GetDataAbsoluteX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::AND_ABS_Y()
{
    uint8_t extra_cycles = 0;
    A &= GetDataAbsoluteY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::AND_IND_X()
{
    uint8_t extra_cycles = 0;
    A &= GetDataIndirectX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::AND_IND_Y()
{
    uint8_t extra_cycles = 0;
    A &= GetDataIndirectY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_IM()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataImmediate(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_ZP()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataZeroPage(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_ZP_X()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataZeroPageX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_ABS()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataAbsolute(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_ABS_X()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataAbsoluteX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_ABS_Y()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataAbsoluteY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_IND_X()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataIndirectX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::EOR_IND_Y()
{
    uint8_t extra_cycles = 0;
    A ^= GetDataIndirectY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_IM()
{
    uint8_t extra_cycles = 0;
    A |= GetDataImmediate(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_ZP()
{
    uint8_t extra_cycles = 0;
    A |= GetDataZeroPage(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_ZP_X()
{
    uint8_t extra_cycles = 0;
    A |= GetDataZeroPageX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_ABS()
{
    uint8_t extra_cycles = 0;
    A |= GetDataAbsolute(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_ABS_X()
{
    uint8_t extra_cycles = 0;
    A |= GetDataAbsoluteX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_ABS_Y()
{
    uint8_t extra_cycles = 0;
    A |= GetDataAbsoluteY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_IND_X()
{
    uint8_t extra_cycles = 0;
    A |= GetDataIndirectX(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ORA_IND_Y()
{
    uint8_t extra_cycles = 0;
    A |= GetDataIndirectY(&extra_cycles);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::BIT_ZP()
{
    uint8_t data = GetDataZeroPage();
    uint8_t result = A & data;
    P.Flags.Z = (result == 0 ? 1 : 0);
    P.Flags.V = checkBit(data, 6);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::BIT_ABS()
{
    uint8_t data = GetDataAbsolute();
    uint8_t result = A & data;

    P.Flags.Z = (result == 0 ? 1 : 0);
    P.Flags.V = checkBit(data, 6);
    P.Flags.N = checkBit(data, 7);

    return 0;
}

uint8_t CPU::ADC_IM()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataImmediate(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ADC_ZP()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPage(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ADC_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPageX(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ADC_ABS()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsolute(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ADC_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsoluteX(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ADC_ABS_Y()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsoluteY(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::ADC_IND_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataIndirectX(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;

    return 0;
}

uint8_t CPU::ADC_IND_Y()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataIndirectY(&extra_cycles);
    A = ADC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_IM()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataImmediate(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_ZP()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPage(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPageX(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_ABS()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsolute(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsoluteX(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_ABS_Y()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsoluteY(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_IND_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataIndirectX(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::SBC_IND_Y()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataIndirectY(&extra_cycles);
    A = SBC(A, data);
    SetNegativeAndZeroFlags(A);

    return extra_cycles;
}

uint8_t CPU::CMP_IM()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataImmediate(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CMP_ZP()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPage(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CMP_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPageX(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CMP_ABS()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsolute(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CMP_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsoluteX(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CMP_ABS_Y()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsoluteY(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CMP_IND_X()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataIndirectX(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CMP_IND_Y()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataIndirectY(&extra_cycles);

    P.Flags.C = (A >= data ? 1 : 0);
    P.Flags.Z = (A == data ? 1 : 0);
    P.Flags.N = checkBit((A - data), 7);

    return extra_cycles;
}

uint8_t CPU::CPX_IM()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataImmediate(&extra_cycles);

    P.Flags.C = (X >= data ? 1 : 0);
    P.Flags.Z = (X == data ? 1 : 0);
    P.Flags.N = checkBit((X - data), 7);

    return extra_cycles;
}

uint8_t CPU::CPX_ZP()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPage(&extra_cycles);

    P.Flags.C = (X >= data ? 1 : 0);
    P.Flags.Z = (X == data ? 1 : 0);
    P.Flags.N = checkBit((X - data), 7);

    return extra_cycles;
}

uint8_t CPU::CPX_ABS()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsolute(&extra_cycles);

    P.Flags.C = (X >= data ? 1 : 0);
    P.Flags.Z = (X == data ? 1 : 0);
    P.Flags.N = checkBit((X - data), 7);

    return extra_cycles;
}

uint8_t CPU::CPY_IM()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataImmediate(&extra_cycles);

    P.Flags.C = (Y >= data ? 1 : 0);
    P.Flags.Z = (Y == data ? 1 : 0);
    P.Flags.N = checkBit((Y - data), 7);

    return extra_cycles;
}

uint8_t CPU::CPY_ZP()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataZeroPage(&extra_cycles);

    P.Flags.C = (Y >= data ? 1 : 0);
    P.Flags.Z = (Y == data ? 1 : 0);
    P.Flags.N = checkBit((Y - data), 7);

    return extra_cycles;
}

uint8_t CPU::CPY_ABS()
{
    uint8_t extra_cycles = 0;
    uint8_t data = GetDataAbsolute(&extra_cycles);

    P.Flags.C = (Y >= data ? 1 : 0);
    P.Flags.Z = (Y == data ? 1 : 0);
    P.Flags.N = checkBit((Y - data), 7);

    return extra_cycles;
}

uint8_t CPU::INC_ZP()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPage(&extra_cycles, &ZP_address) + 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::INC_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPageX(&extra_cycles, &ZP_address) + 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::INC_ABS()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsolute(&extra_cycles, &address) + 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::INC_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsoluteX(&extra_cycles, &address) + 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::INX()
{
    ++X;
    SetNegativeAndZeroFlags(X);

    return 0;
}

uint8_t CPU::INY()
{
    ++Y;
    SetNegativeAndZeroFlags(Y);

    return 0;
}

uint8_t CPU::DEC_ZP()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPage(&extra_cycles, &ZP_address) - 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::DEC_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPageX(&extra_cycles, &ZP_address) - 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::DEC_ABS()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsolute(&extra_cycles, &address) - 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::DEC_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsoluteX(&extra_cycles, &address) - 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::DEX()
{
    --X;
    SetNegativeAndZeroFlags(X);

    return 0;
}

uint8_t CPU::DEY()
{
    --Y;
    SetNegativeAndZeroFlags(Y);

    return 0;
}

uint8_t CPU::ASL_ACC()
{
    P.Flags.C = checkBit(A, 7);
    A <<= 1;
    SetNegativeAndZeroFlags(A);

    return 0;
}

uint8_t CPU::ASL_ZP()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPage(&extra_cycles, &ZP_address);
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ASL_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPageX(&extra_cycles, &ZP_address);
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ASL_ABS()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsolute(&extra_cycles, &address);
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ASL_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsoluteX(&extra_cycles, &address);
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::LSR_ACC()
{
    P.Flags.C = checkBit(A, 0);
    A >>= 1;
    SetNegativeAndZeroFlags(A);

    return 0;
}

uint8_t CPU::LSR_ZP()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPage(&extra_cycles, &ZP_address);
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::LSR_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPageX(&extra_cycles, &ZP_address);
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::LSR_ABS()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsolute(&extra_cycles, &address);
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::LSR_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsoluteX(&extra_cycles, &address);
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROL_ACC()
{
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(A, 7);
    A <<= 1;
    if (old_carry)
        setBit(A, 0);

    SetNegativeAndZeroFlags(A);

    return 0;
}

uint8_t CPU::ROL_ZP()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPage(&extra_cycles, &ZP_address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROL_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPageX(&extra_cycles, &ZP_address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROL_ABS()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsolute(&extra_cycles, &address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROL_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsoluteX(&extra_cycles, &address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 7);
    data <<= 1;
    if (old_carry)
        setBit(data, 0);
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROR_ACC()
{
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(A, 0);
    A >>= 1;
    if (old_carry)
        setBit(A, 7);

    SetNegativeAndZeroFlags(A);

    return 0;
}

uint8_t CPU::ROR_ZP()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPage(&extra_cycles, &ZP_address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROR_ZP_X()
{
    uint8_t extra_cycles = 0;
    uint16_t ZP_address;
    uint8_t data = GetDataZeroPageX(&extra_cycles, &ZP_address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);
    SetByte(ZP_address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROR_ABS()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsolute(&extra_cycles, &address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
}

uint8_t CPU::ROR_ABS_X()
{
    uint8_t extra_cycles = 0;
    uint16_t address;
    uint8_t data = GetDataAbsoluteX(&extra_cycles, &address);
    uint8_t old_carry = P.Flags.C;
    P.Flags.C = checkBit(data, 0);
    data >>= 1;
    if (old_carry)
        setBit(data, 7);
    SetByte(address, data);
    SetNegativeAndZeroFlags(data);

    return extra_cycles;
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
    PC = GetWordFromAddress(IRQ_VECTOR);

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

uint8_t CPU::RESET()
{
    PC = GetWordFromAddress(RESET_VECTOR);
    SP = 0xFF;
    A = 0;
    X = 0;
    Y = 0;

    P.Pbyte = 0b00000000;

    return 8;
}

uint8_t CPU::NMI()
{
    P.Flags.U = 1;

    PushWordToStack(PC);
    PushByteToStack(P.Pbyte);
    PC = GetWordFromAddress(NMI_VECTOR);

    P.Flags.I = 1;

    return 8;
}

uint8_t CPU::IRQ()
{
    P.Flags.U = 1;

    PushWordToStack(PC);
    PushByteToStack(P.Pbyte);
    PC = GetWordFromAddress(IRQ_VECTOR);

    P.Flags.I = 1;

    return 8;
}
