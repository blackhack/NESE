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

#ifndef Cartridge_h__
#define Cartridge_h__

#include <vector>
#include <string>

class Cartridge
{
public:
    Cartridge(std::string rom_path);
    ~Cartridge();

    struct NES_2_0
    {
        uint8_t ID[4];
        uint8_t PGR_ROM_LSB;
        uint8_t CHR_ROM_LSB;
        uint8_t Flag_6;
        uint8_t Flag_7;
        uint8_t Flag_8;
        uint8_t PGR_CHR_ROM_MSB;
        uint8_t PGR_EEPROM_SIZE;
        uint8_t CHR_RAM_SIZE;
        uint8_t Timing;
        uint8_t VS_System;
        uint8_t Flag_14;
        uint8_t Flag_15;
    } format_header;

    enum class FormatType
    {
        Unk,
        iNES,
        NES20,
    };
    FormatType detected_format;

    std::vector<uint8_t> Trainer;
    std::vector<uint8_t> PGR_ROM;
    std::vector<uint8_t> CHR_ROM_RAM;
};

#endif // Cartridge_h__
