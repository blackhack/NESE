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

#include "Cartridge.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

Cartridge::Cartridge(std::string rom_path)
{
    std::ifstream file_stream;
    file_stream.open(rom_path, std::ios::binary | std::ios::ate);
    std::streamoff file_size = file_stream.tellg();
    file_stream.seekg(0, std::ios::beg);

    if (!file_stream.is_open())
    {
        std::cerr << "Cant open the file\n";
        return;
    }

    // Load header, the struct is already aligned, so no padding risk.
    file_stream.read((char*)&format_header, sizeof(format_header));

    detected_format = FormatType::Unk;

    if (format_header.ID[0] == 'N' && format_header.ID[1] == 'E' && format_header.ID[2] == 'S' && format_header.ID[3] == 0x1A)
    {
        detected_format = FormatType::iNES;

        if ((format_header.Flag_7 & 0x0C) == 0x08)
            detected_format = FormatType::NES20;
    }

    // Load trainer data if present
    if ((format_header.Flag_6 & 0b00000100) != 0)
        file_stream.read((char*)Trainer.data(), 512);

    switch (detected_format)
    {
    case Cartridge::FormatType::iNES:
    {
        uint8_t PGR_16KB_units = format_header.PGR_ROM_LSB | ((format_header.PGR_CHR_ROM_MSB & 0x0F) << 8);
        uint8_t CHR_8KB_units = format_header.CHR_ROM_LSB;

        //When CHR_8KB_units == 0 then it uses CHR RAM instead of ROM
        if (CHR_8KB_units == 0)
            CHR_8KB_units = 1;

        PGR_ROM.resize(PGR_16KB_units * 16384);
        file_stream.read((char*)PGR_ROM.data(), PGR_16KB_units * 16384);

        CHR_ROM_RAM.resize(CHR_8KB_units * 8192);
        file_stream.read((char*)CHR_ROM_RAM.data(), CHR_8KB_units * 8192);

        break;
    }
    case Cartridge::FormatType::NES20:
    {
        //@todo: I think this is wrong, need to read more the documentation
        uint16_t PGR_size = format_header.PGR_ROM_LSB | (format_header.PGR_CHR_ROM_MSB << 8);
        uint16_t CHR_size = format_header.CHR_ROM_LSB | (format_header.PGR_CHR_ROM_MSB & 0xF0);

        //When CHR_8KB_units == 0 then it uses CHR RAM instead of ROM
        if (CHR_size == 0)
            CHR_size = 8192;

        PGR_ROM.resize(PGR_size);
        file_stream.read((char*)PGR_ROM.data(), PGR_size);

        CHR_ROM_RAM.resize(CHR_size);
        file_stream.read((char*)CHR_ROM_RAM.data(), CHR_size);

        break;
    }
    default:
        std::cerr << "ERROR> Trying to load unknown format type.\n";
        exit(1);
        break;
    }

    std::cout << "ROM Loaded> " << "Trainer: " << Trainer.size() << " bytes - PGR_ROM: "
        << PGR_ROM.size() << " bytes - CHR_ROM_RAM: " << CHR_ROM_RAM.size() << " bytes - Type: "
        << (detected_format == FormatType::iNES ? "iNES\n" : "NES 2.0\n");

    std::cout << "MISC Size: " << (file_size - sizeof(format_header) - PGR_ROM.size() - CHR_ROM_RAM.size()) << " bytes\n";
}

Cartridge::~Cartridge()
{

}
