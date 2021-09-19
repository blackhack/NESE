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

#ifndef CPU_h__
#define CPU_h__

#include <cstdint>
#include "Opcode.h"
#include "Memory.h"

#define checkBit(var, pos) ((var >> pos) & 0x1);
#define setBit(var, pos) var |= (0x1 << pos)
#define clearBit(var, pos) var &= ~(0x1 << pos)

class CPU
{
public:
    CPU(Memory& mem, uint32_t frequency);

    void Reset();

    /* Registers Set*/
    uint16_t PC; // ProgramCounter
    uint8_t SP; // StackPointer
    uint8_t A; // Accumulator
    uint8_t X; // Index Register X
    uint8_t Y; // Index Register Y

    union
    {
        struct ProcessorStatus
        {
            uint8_t C : 1; // Carry Flag
            uint8_t Z : 1; // Zero Flag
            uint8_t I : 1; // Interrupt Disable
            uint8_t D : 1; // Decimal Mode
            uint8_t B : 1; // Break Command
            uint8_t : 1; // Unused
            uint8_t V : 1; // Overflow Flag
            uint8_t N : 1; // Negative Flag
        } flags;

        uint8_t PS_byte;
    } PS;

    uint32_t Execute(uint32_t instructions_to_execute);
    uint8_t CPU::GetByteFromPC();
    uint16_t CPU::GetWordFromPC();
    uint8_t CPU::GetByteFromAddress(uint16_t address);
    uint16_t CPU::GetWordFromAddress(uint16_t address);

    void SetByte(uint16_t address, uint8_t data);
    void SetWord(uint16_t address, uint16_t data);

    void PushByteToStack(uint8_t data);
    void PushWordToStack(uint16_t data);
    uint8_t PullByteFromStack();
    uint16_t PullWordFromStack();


    uint8_t LDA_IM();
    uint8_t LDA_ZP();
    uint8_t LDA_ZP_X();
    uint8_t LDA_ABS();
    uint8_t LDA_ABS_X();
    uint8_t LDA_ABS_Y();
    uint8_t LDA_IND_X();
    uint8_t LDA_IND_Y();

    uint8_t LDX_IM();
    uint8_t LDX_ZP();
    uint8_t LDX_ZP_Y();
    uint8_t LDX_ABS();
    uint8_t LDX_ABS_Y();

    uint8_t LDY_IM();
    uint8_t LDY_ZP();
    uint8_t LDY_ZP_X();
    uint8_t LDY_ABS();
    uint8_t LDY_ABS_X();

    uint8_t STA_ZP();
    uint8_t STA_ZP_X();
    uint8_t STA_ABS();
    uint8_t STA_ABS_X();
    uint8_t STA_ABS_Y();
    uint8_t STA_IND_X();
    uint8_t STA_IND_Y();

    uint8_t STX_ZP();
    uint8_t STX_ZP_Y();
    uint8_t STX_ABS();

    uint8_t STY_ZP();
    uint8_t STY_ZP_X();
    uint8_t STY_ABS();

    uint8_t TAX();
    uint8_t TAY();
    uint8_t TXA();
    uint8_t TYA();

    uint8_t TSX();
    uint8_t TXS();
    uint8_t PHA();
    uint8_t PHP();
    uint8_t PLA();
    uint8_t PLP();

    uint8_t AND_IM();
    uint8_t AND_ZP();
    uint8_t AND_ZP_X();
    uint8_t AND_ABS();
    uint8_t AND_ABS_X();
    uint8_t AND_ABS_Y();
    uint8_t AND_IND_X();
    uint8_t AND_IND_Y();

    uint8_t EOR_IM();
    uint8_t EOR_ZP();
    uint8_t EOR_ZP_X();
    uint8_t EOR_ABS();
    uint8_t EOR_ABS_X();
    uint8_t EOR_ABS_Y();
    uint8_t EOR_IND_X();
    uint8_t EOR_IND_Y();

    uint8_t ORA_IM();
    uint8_t ORA_ZP();
    uint8_t ORA_ZP_X();
    uint8_t ORA_ABS();
    uint8_t ORA_ABS_X();
    uint8_t ORA_ABS_Y();
    uint8_t ORA_IND_X();
    uint8_t ORA_IND_Y();

    uint8_t BIT_ZP();
    uint8_t BIT_ABS();

    uint8_t ADC(uint8_t a, uint8_t b);
    uint8_t SBC(uint8_t a, uint8_t b);

    uint8_t ADC_IM();
    uint8_t ADC_ZP();
    uint8_t ADC_ZP_X();
    uint8_t ADC_ABS();
    uint8_t ADC_ABS_X();
    uint8_t ADC_ABS_Y();
    uint8_t ADC_IND_X();
    uint8_t ADC_IND_Y();

    uint8_t SBC_IM();
    uint8_t SBC_ZP();
    uint8_t SBC_ZP_X();
    uint8_t SBC_ABS();
    uint8_t SBC_ABS_X();
    uint8_t SBC_ABS_Y();
    uint8_t SBC_IND_X();
    uint8_t SBC_IND_Y();

    uint8_t CMP_IM();
    uint8_t CMP_ZP();
    uint8_t CMP_ZP_X();
    uint8_t CMP_ABS();
    uint8_t CMP_ABS_X();
    uint8_t CMP_ABS_Y();
    uint8_t CMP_IND_X();
    uint8_t CMP_IND_Y();

    uint8_t CPX_IM();
    uint8_t CPX_ZP();
    uint8_t CPX_ABS();

    uint8_t CPY_IM();
    uint8_t CPY_ZP();
    uint8_t CPY_ABS();

    uint8_t INC_ZP();
    uint8_t INC_ZP_X();
    uint8_t INC_ABS();
    uint8_t INC_ABS_X();

    uint8_t INX();
    uint8_t INY();

    uint8_t DEC_ZP();
    uint8_t DEC_ZP_X();
    uint8_t DEC_ABS();
    uint8_t DEC_ABS_X();

    uint8_t DEX();
    uint8_t DEY();

    uint8_t ASL_ACC();
    uint8_t ASL_ZP();
    uint8_t ASL_ZP_X();
    uint8_t ASL_ABS();
    uint8_t ASL_ABS_X();

    uint8_t LSR_ACC();
    uint8_t LSR_ZP();
    uint8_t LSR_ZP_X();
    uint8_t LSR_ABS();
    uint8_t LSR_ABS_X();

    uint8_t ROL_ACC();
    uint8_t ROL_ZP();
    uint8_t ROL_ZP_X();
    uint8_t ROL_ABS();
    uint8_t ROL_ABS_X();

    uint8_t ROR_ACC();
    uint8_t ROR_ZP();
    uint8_t ROR_ZP_X();
    uint8_t ROR_ABS();
    uint8_t ROR_ABS_X();

    uint8_t JMP_ABS();
    uint8_t JMP_IND();

    uint8_t JSR_ABS();

    uint8_t RTS();

    Memory& memory;
    uint64_t cycle_period_ns;
};

#endif // CPU_h__
