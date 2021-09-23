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

#include "Bus.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

Bus::Bus()
{
    _data.resize(MAX_MEMORY, 0);
}

Bus::~Bus()
{

}

const uint8_t Bus::operator[](uint16_t address) const
{
    return _data.at(address);
}

uint8_t& Bus::operator[](uint16_t address)
{
    return _data.at(address);
}

bool Bus::LoadFile(std::string filepath)
{
    std::ifstream fileStream;
    fileStream.open(filepath, std::ios::binary | std::ios::ate);
    std::streamoff fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    if (!fileStream.is_open())
    {
        std::cerr << "Cant open the file\n";
        return false;
    }

    if (fileSize > MAX_MEMORY)
    {
        std::cerr << "The rom is too big: " << fileSize << " bytes (Maximum: 65536)\n";
        return false;
    }

    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(fileStream), {});

    for (uint32_t i = 0; i < buffer.size(); ++i)
    {
        uint8_t value = buffer[i];
        _data[0x0000000A + i] = value;
    }

    std::cout << "ROM loaded!, Size: " << fileSize << " bytes" << std::endl;

    return true;
}
