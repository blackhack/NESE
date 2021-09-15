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

#ifndef Memory_h__
#define Memory_h__

#include <cstdint>
#include <array>

constexpr uint16_t MAX_MEMORY = 0xFFFF;

class Memory
{
public:
    Memory();

    const uint8_t operator[](uint16_t address) const;
    uint8_t& operator[](uint16_t address);

private:
    std::array<uint8_t, MAX_MEMORY> _data;
};

#endif // Memory_h__