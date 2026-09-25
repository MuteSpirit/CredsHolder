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
#include "eeprom_storage.hpp"

#define RESET_FLAG (0xff)

EepromStorage::EepromStorage(const uint8_t csPin, const unsigned int wpPin, const unsigned int speed)
    : eep_(csPin, wpPin, speed)
{}

bool
EepromStorage::init(const enum eeprom_size_t fullSizeBytes, const enum EEPROM_WE_PAGE_SIZE pageSize)
{
    if (!eep_.init()) {
        return false;
    }

    eep_.setPageSize(pageSize);
    eep_.setMemorySize(fullSizeBytes);

    pageSize_ = 16 << pageSize;
    fullSizeBytes_ = (uint32_t)fullSizeBytes;

    if (RESET_FLAG != getResetFlag()) {
        factoryReset();
    }

    return true;
}

uint8_t
EepromStorage::getResetFlag()
{
    return eep_.read(fullSizeBytes_ - 1);
}

void
EepromStorage::setResetFlag()
{
    eep_.write(fullSizeBytes_ - 1, RESET_FLAG);
}
size_t
EepromStorage::minAddr() const
{
    return 0;
}

size_t
EepromStorage::maxAddr() const
{
    return fullSizeBytes_ - 1 /* init flag size */;
}

bool
EepromStorage::isAddrOk(const size_t addr) const
{
    return minAddr() <= addr && addr < maxAddr();
}

uint8_t
EepromStorage::read(const size_t addr)
{
    return eep_.read(addr);
}

void
EepromStorage::write(const size_t addr, const uint8_t b)
{
    if (isAddrOk(addr)) {
        eep_.write(addr, b);
    }
}

void
EepromStorage::read(const size_t addr, uint8_t *buf, const size_t size)
{
    for (uint8_t i = 0; i < size && isAddrOk(addr + i); ++i) {
        buf[i] = eep_.read(addr + i);
    }
}

void
EepromStorage::write(const size_t addr, const uint8_t *buf, const size_t size)
{
    for (uint8_t i = 0; isAddrOk(addr + i); ++i) {
        eep_.write(addr + i, buf[i]);
    }
}

void
EepromStorage::factoryReset()
{
    eep_.eraseCompleteEEPROM();
    setResetFlag();
}
