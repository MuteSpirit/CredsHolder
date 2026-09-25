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

#include "block_storage.hpp"

class InternalEepromStorage : public BlockStorage
{
public:
    InternalEepromStorage();
    ~InternalEepromStorage() = default;
    bool init();

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
    unsigned int fullSizeBytes_; // TODO: is it needed to cache that value or use EEPROM.length() directly?
};
