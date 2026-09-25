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

// TODO: use size_t instead of uint32_t as address type

class BlockStorage;


template<typename Object>
class ModelStorage
{
public:
    using ObjIndex = uint16_t;

public:
    ModelStorage(BlockStorage &bs) : bs_(bs) {};
    ~ModelStorage() = default;

    bool isExist(const char* key);
    bool search(const char* key, Object &t);

    ObjIndex count() const;
    ObjIndex maxIdx() const;

    bool get(const ObjIndex idx, Object &t);
    bool getNext(const ObjIndex from, Object &t, ObjIndex &idx);
    bool getPrev(const ObjIndex from, Object &t, ObjIndex &idx);
    bool add(const Object &t);
    bool del(const char* key);

    void factoryReset();

protected:
    /// @brief Store not only original model object but also special flag.
    /// meaning fully written data about object.
    /// Before any edit operation with object "commitFlag_" must be set to 0x0.
    /// After finish operation set 0x1 again.
    struct __attribute__((packed)) ObjInStorage
    {
        enum class Committment : uint8_t
        {
            free     = 0,
            comitted = 1,
            COUNT
        };

        Object obj_;
        uint8_t reserve_[15];
        uint8_t commitFlag_;
    };

protected:
    bool isOkIdx(const ObjIndex idx) const;
    bool isFreeAccount(const ObjIndex idx) const;
    bool getFreeObjectIndex(ObjIndex &idx);
    size_t idx2addr(const ObjIndex idx) const;
    size_t getKeyAddr(const ObjIndex idx);

protected:
    BlockStorage &bs_;
};
