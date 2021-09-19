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

#include "Opcode.h"
#include "CPU.h"

OpcodeHandler::OpcodeHandler() : name(""), callback(nullptr)
{
}

OpcodeHandler::OpcodeHandler(std::string opName, uint8_t opBase_cycles, OpcodeCallback opCallback)
{
    name = opName;
    base_cycles = opBase_cycles;
    callback = opCallback;
}

#define DEFINE_HANDLER(opcode, base_cycles, callback) { Opcode::opcode, OpcodeHandler(#opcode, base_cycles, callback) }

std::map<Opcode, OpcodeHandler> opcodesHandlers = {
    DEFINE_HANDLER(LDA_IM,    2, &CPU::LDA_IM),
    DEFINE_HANDLER(LDA_ZP,    3, &CPU::LDA_ZP),
    DEFINE_HANDLER(LDA_ZP_X,  4, &CPU::LDA_ZP_X),
    DEFINE_HANDLER(LDA_ABS,   4, &CPU::LDA_ABS),
    DEFINE_HANDLER(LDA_ABS_X, 4, &CPU::LDA_ABS_X),
    DEFINE_HANDLER(LDA_ABS_Y, 4, &CPU::LDA_ABS_Y),
    DEFINE_HANDLER(LDA_IND_X, 6, &CPU::LDA_IND_X),
    DEFINE_HANDLER(LDA_IND_Y, 5, &CPU::LDA_IND_Y),

    DEFINE_HANDLER(LDX_IM,    2, &CPU::LDX_IM),
    DEFINE_HANDLER(LDX_ZP,    3, &CPU::LDX_ZP),
    DEFINE_HANDLER(LDX_ZP_Y,  4, &CPU::LDX_ZP_Y),
    DEFINE_HANDLER(LDX_ABS,   4, &CPU::LDX_ABS),
    DEFINE_HANDLER(LDX_ABS_Y, 4, &CPU::LDX_ABS_Y),

    DEFINE_HANDLER(LDY_IM,    2, &CPU::LDY_IM),
    DEFINE_HANDLER(LDY_ZP,    3, &CPU::LDY_ZP),
    DEFINE_HANDLER(LDY_ZP_X,  4, &CPU::LDY_ZP_X),
    DEFINE_HANDLER(LDY_ABS,   4, &CPU::LDY_ABS),
    DEFINE_HANDLER(LDY_ABS_X, 4, &CPU::LDY_ABS_X),

    DEFINE_HANDLER(STA_ZP,    3, &CPU::STA_ZP),
    DEFINE_HANDLER(STA_ZP_X,  4, &CPU::STA_ZP_X),
    DEFINE_HANDLER(STA_ABS,   4, &CPU::STA_ABS),
    DEFINE_HANDLER(STA_ABS_X, 5, &CPU::STA_ABS_X),
    DEFINE_HANDLER(STA_ABS_Y, 5, &CPU::STA_ABS_Y),
    DEFINE_HANDLER(STA_IND_X, 6, &CPU::STA_IND_X),
    DEFINE_HANDLER(STA_IND_Y, 6, &CPU::STA_IND_Y),

    DEFINE_HANDLER(STX_ZP,    3, &CPU::STX_ZP),
    DEFINE_HANDLER(STX_ZP_Y,  4, &CPU::STX_ZP_Y),
    DEFINE_HANDLER(STX_ABS,   4, &CPU::STX_ABS),

    DEFINE_HANDLER(STY_ZP,    3, &CPU::STY_ZP),
    DEFINE_HANDLER(STY_ZP_X,  4, &CPU::STY_ZP_X),
    DEFINE_HANDLER(STY_ABS,   4, &CPU::STY_ABS),

    DEFINE_HANDLER(TAX,       2, &CPU::TAX),
    DEFINE_HANDLER(TAY,       2, &CPU::TAY),
    DEFINE_HANDLER(TXA,       2, &CPU::TXA),
    DEFINE_HANDLER(TYA,       2, &CPU::TYA),

    DEFINE_HANDLER(TSX,       2, &CPU::TSX),
    DEFINE_HANDLER(TXS,       2, &CPU::TXS),
    DEFINE_HANDLER(PHA,       3, &CPU::PHA),
    DEFINE_HANDLER(PHP,       3, &CPU::PHP),
    DEFINE_HANDLER(PLA,       4, &CPU::PLA),
    DEFINE_HANDLER(PLP,       4, &CPU::PLP),

    DEFINE_HANDLER(AND_IM,    2, &CPU::AND_IM),
    DEFINE_HANDLER(AND_ZP,    3, &CPU::AND_ZP),
    DEFINE_HANDLER(AND_ZP_X,  4, &CPU::AND_ZP_X),
    DEFINE_HANDLER(AND_ABS,   4, &CPU::AND_ABS),
    DEFINE_HANDLER(AND_ABS_X, 4, &CPU::AND_ABS_X),
    DEFINE_HANDLER(AND_ABS_Y, 4, &CPU::AND_ABS_Y),
    DEFINE_HANDLER(AND_IND_X, 6, &CPU::AND_IND_X),
    DEFINE_HANDLER(AND_IND_Y, 5, &CPU::AND_IND_Y),

    DEFINE_HANDLER(EOR_IM,    2, &CPU::EOR_IM),
    DEFINE_HANDLER(EOR_ZP,    3, &CPU::EOR_ZP),
    DEFINE_HANDLER(EOR_ZP_X,  4, &CPU::EOR_ZP_X),
    DEFINE_HANDLER(EOR_ABS,   4, &CPU::EOR_ABS),
    DEFINE_HANDLER(EOR_ABS_X, 4, &CPU::EOR_ABS_X),
    DEFINE_HANDLER(EOR_ABS_Y, 4, &CPU::EOR_ABS_Y),
    DEFINE_HANDLER(EOR_IND_X, 6, &CPU::EOR_IND_X),
    DEFINE_HANDLER(EOR_IND_Y, 5, &CPU::EOR_IND_Y),

    DEFINE_HANDLER(ORA_IM,    2, &CPU::ORA_IM),
    DEFINE_HANDLER(ORA_ZP,    3, &CPU::ORA_ZP),
    DEFINE_HANDLER(ORA_ZP_X,  4, &CPU::ORA_ZP_X),
    DEFINE_HANDLER(ORA_ABS,   4, &CPU::ORA_ABS),
    DEFINE_HANDLER(ORA_ABS_X, 4, &CPU::ORA_ABS_X),
    DEFINE_HANDLER(ORA_ABS_Y, 4, &CPU::ORA_ABS_Y),
    DEFINE_HANDLER(ORA_IND_X, 6, &CPU::ORA_IND_X),
    DEFINE_HANDLER(ORA_IND_Y, 5, &CPU::ORA_IND_Y),

    DEFINE_HANDLER(BIT_ZP,    3, &CPU::BIT_ZP),
    DEFINE_HANDLER(BIT_ABS,   4, &CPU::BIT_ABS),

    DEFINE_HANDLER(ADC_IM,    2, &CPU::ADC_IM),
    DEFINE_HANDLER(ADC_ZP,    3, &CPU::ADC_ZP),
    DEFINE_HANDLER(ADC_ZP_X,  4, &CPU::ADC_ZP_X),
    DEFINE_HANDLER(ADC_ABS,   4, &CPU::ADC_ABS),
    DEFINE_HANDLER(ADC_ABS_X, 4, &CPU::ADC_ABS_X),
    DEFINE_HANDLER(ADC_ABS_Y, 4, &CPU::ADC_ABS_Y),
    DEFINE_HANDLER(ADC_IND_X, 6, &CPU::ADC_IND_X),
    DEFINE_HANDLER(ADC_IND_Y, 5, &CPU::ADC_IND_Y),

    DEFINE_HANDLER(SBC_IM,    2, &CPU::SBC_IM),
    DEFINE_HANDLER(SBC_ZP,    3, &CPU::SBC_ZP),
    DEFINE_HANDLER(SBC_ZP_X,  4, &CPU::SBC_ZP_X),
    DEFINE_HANDLER(SBC_ABS,   4, &CPU::SBC_ABS),
    DEFINE_HANDLER(SBC_ABS_X, 4, &CPU::SBC_ABS_X),
    DEFINE_HANDLER(SBC_ABS_Y, 4, &CPU::SBC_ABS_Y),
    DEFINE_HANDLER(SBC_IND_X, 6, &CPU::SBC_IND_X),
    DEFINE_HANDLER(SBC_IND_Y, 5, &CPU::SBC_IND_Y),

    DEFINE_HANDLER(CMP_IM,    2, &CPU::CMP_IM),
    DEFINE_HANDLER(CMP_ZP,    3, &CPU::CMP_ZP),
    DEFINE_HANDLER(CMP_ZP_X,  4, &CPU::CMP_ZP_X),
    DEFINE_HANDLER(CMP_ABS,   4, &CPU::CMP_ABS),
    DEFINE_HANDLER(CMP_ABS_X, 4, &CPU::CMP_ABS_X),
    DEFINE_HANDLER(CMP_ABS_Y, 4, &CPU::CMP_ABS_Y),
    DEFINE_HANDLER(CMP_IND_X, 6, &CPU::CMP_IND_X),
    DEFINE_HANDLER(CMP_IND_Y, 5, &CPU::CMP_IND_Y),

    DEFINE_HANDLER(CPX_IM,    2, &CPU::CPX_IM),
    DEFINE_HANDLER(CPX_ZP,    3, &CPU::CPX_ZP),
    DEFINE_HANDLER(CPX_ABS,   4, &CPU::CPX_ABS),

    DEFINE_HANDLER(CPY_IM,    2, &CPU::CPY_IM),
    DEFINE_HANDLER(CPY_ZP,    3, &CPU::CPY_ZP),
    DEFINE_HANDLER(CPY_ABS,   4, &CPU::CPY_ABS),

    DEFINE_HANDLER(INC_ZP,    5, &CPU::INC_ZP),
    DEFINE_HANDLER(INC_ZP_X,  6, &CPU::INC_ZP_X),
    DEFINE_HANDLER(INC_ABS,   6, &CPU::INC_ABS),
    DEFINE_HANDLER(INC_ABS_X, 7, &CPU::INC_ABS_X),

    DEFINE_HANDLER(INX,       2, &CPU::INX),
    DEFINE_HANDLER(INY,       2, &CPU::INY),

    DEFINE_HANDLER(DEC_ZP,    5, &CPU::DEC_ZP),
    DEFINE_HANDLER(DEC_ZP_X,  6, &CPU::DEC_ZP_X),
    DEFINE_HANDLER(DEC_ABS,   6, &CPU::DEC_ABS),
    DEFINE_HANDLER(DEC_ABS_X, 7, &CPU::DEC_ABS_X),

    DEFINE_HANDLER(DEX,       2, &CPU::DEX),
    DEFINE_HANDLER(DEY,       2, &CPU::DEY),

    DEFINE_HANDLER(ASL_ACC,   2, &CPU::ASL_ACC),
    DEFINE_HANDLER(ASL_ZP,    5, &CPU::ASL_ZP),
    DEFINE_HANDLER(ASL_ZP_X,  6, &CPU::ASL_ZP_X),
    DEFINE_HANDLER(ASL_ABS,   6, &CPU::ASL_ABS),
    DEFINE_HANDLER(ASL_ABS_X, 7, &CPU::ASL_ABS_X),

    DEFINE_HANDLER(LSR_ACC,   2, &CPU::LSR_ACC),
    DEFINE_HANDLER(LSR_ZP,    5, &CPU::LSR_ZP),
    DEFINE_HANDLER(LSR_ZP_X,  6, &CPU::LSR_ZP_X),
    DEFINE_HANDLER(LSR_ABS,   6, &CPU::LSR_ABS),
    DEFINE_HANDLER(LSR_ABS_X, 7, &CPU::LSR_ABS_X),

    DEFINE_HANDLER(ROL_ACC,   2, &CPU::ROL_ACC),
    DEFINE_HANDLER(ROL_ZP,    5, &CPU::ROL_ZP),
    DEFINE_HANDLER(ROL_ZP_X,  6, &CPU::ROL_ZP_X),
    DEFINE_HANDLER(ROL_ABS,   6, &CPU::ROL_ABS),
    DEFINE_HANDLER(ROL_ABS_X, 7, &CPU::ROL_ABS_X),

    DEFINE_HANDLER(ROR_ACC,   2, &CPU::ROR_ACC),
    DEFINE_HANDLER(ROR_ZP,    5, &CPU::ROR_ZP),
    DEFINE_HANDLER(ROR_ZP_X,  6, &CPU::ROR_ZP_X),
    DEFINE_HANDLER(ROR_ABS,   6, &CPU::ROR_ABS),
    DEFINE_HANDLER(ROR_ABS_X, 7, &CPU::ROR_ABS_X),

    DEFINE_HANDLER(JMP_ABS,   3, &CPU::JMP_ABS),
    DEFINE_HANDLER(JMP_IND,   5, &CPU::JMP_IND),
    DEFINE_HANDLER(JSR_ABS,   6, &CPU::JSR_ABS),
    DEFINE_HANDLER(RTS,       6, &CPU::RTS),

    DEFINE_HANDLER(BCC_REL,   2, &CPU::BCC_REL),
    DEFINE_HANDLER(BCS_REL,   2, &CPU::BCS_REL),
    DEFINE_HANDLER(BEQ_REL,   2, &CPU::BEQ_REL),
    DEFINE_HANDLER(BMI_REL,   2, &CPU::BMI_REL),
    DEFINE_HANDLER(BNE_REL,   2, &CPU::BNE_REL),
    DEFINE_HANDLER(BPL_REL,   2, &CPU::BPL_REL),
    DEFINE_HANDLER(BVC_REL,   2, &CPU::BVC_REL),
    DEFINE_HANDLER(BVS_REL,   2, &CPU::BVS_REL),

    DEFINE_HANDLER(CLC,       2, &CPU::CLC),
    DEFINE_HANDLER(CLD,       2, &CPU::CLD),
    DEFINE_HANDLER(CLI,       2, &CPU::CLI),
    DEFINE_HANDLER(CLV,       2, &CPU::CLV),
    DEFINE_HANDLER(SEC,       2, &CPU::SEC),
    DEFINE_HANDLER(SED,       2, &CPU::SED),
    DEFINE_HANDLER(SEI,       2, &CPU::SEI),

    DEFINE_HANDLER(BRK,       7, &CPU::BRK),
    DEFINE_HANDLER(NOP,       2, &CPU::NOP),
    DEFINE_HANDLER(RTI,       6, &CPU::RTI),
};
