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
#include "creds_holder.hpp"

#include <inttypes.h>

#include <Arduino.h>
#include <EEPROM_SPI_WE.h>

#include "block_storage.hpp"

// TODO: make "EEPROM write protect pin manageble and optimize bulk write operations"
// TODO: make [minAddr, maxAddr] diapasone configurable to store different model objects in different non-overlapped diapasones

class EepromStorage : public BlockStorage
{
public:
    EepromStorage(const uint8_t csPin, const unsigned int wpPin = 999, const unsigned int speed = F_CPU);
    ~EepromStorage() = default;

    bool init(const enum eeprom_size_t fullSizeBytes, const enum EEPROM_WE_PAGE_SIZE pageSize);

public:
    virtual size_t minAddr() const override;
    virtual size_t maxAddr() const override;

    virtual bool isAddrOk(const size_t addr) const override;

    virtual uint8_t read(const size_t addr) override;
    virtual void write(const size_t addr, const uint8_t b) override;

    virtual void read(const size_t addr, uint8_t *buf, const size_t size) override;
    virtual void write(const size_t addr, const uint8_t *buf, const size_t size) override;

    virtual void factoryReset() override;

protected:
    uint8_t getResetFlag();
    void setResetFlag();

protected:
    EEPROM_SPI_WE eep_;
    uint32_t fullSizeBytes_;
    uint16_t pageSize_;
};
