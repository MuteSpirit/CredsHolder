// CredsHolder (Hardware Credential Manager)
// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#pragma once

#include <inttypes.h>
#include <stdio.h>


class BlockStorage
{
public:
    virtual size_t minAddr() const = 0;
    virtual size_t maxAddr() const = 0;

    virtual bool isAddrOk(const size_t addr) const = 0;

    virtual uint8_t read(const size_t addr) = 0;
    virtual void write(const size_t addr, const uint8_t b) = 0;

    virtual void read(const size_t addr, uint8_t *buf, const size_t size) = 0;
    virtual void write(const size_t addr, const uint8_t *buf, const size_t size) = 0;

    template<typename T>
    void write(const size_t addr, const T& t);

    template<typename T>
    void read(const size_t addr, T& t);

    virtual void factoryReset() = 0;

    virtual ~BlockStorage() {};

protected:
    // TODO: ctor must accept min and max available addresses:
    //   Reason: it'll be able to use different EEPROM diapasons
    //   for different object occupation
    BlockStorage() = default;
};

////////////////////////////////////////////////////////////////////////////////
template<typename T>
void BlockStorage::write(const size_t addr, const T& t)
{
    write(addr, reinterpret_cast<const uint8_t*>(&t), sizeof(T));
}

template<typename T>
void BlockStorage::read(const size_t addr, T& t)
{
    read(addr, reinterpret_cast<uint8_t*>(&t), sizeof(T));
}
