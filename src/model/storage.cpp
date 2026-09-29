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
#include "storage.hpp"
#include <string.h>
#include <sys/types.h>
#include "account.hpp"
#include "../block_storage.hpp"

////////////////////////////////////////////////////////////////////////////////
// Let's instantiate for Account to keep control on ModelStorage class usage
template class ModelStorage<Account>;

////////////////////////////////////////////////////////////////////////////////
template<typename T>
bool
ModelStorage<T>::isExist(const char* key)
{
    T o;  
    for (ObjIndex i = 0; isOkIdx(i); ++i) {
        if (isFreeAccount(i)) {
            continue;
        }

        bs_.read(getKeyAddr(i), reinterpret_cast<uint8_t*>(get_key_ptr(o)), get_key_size<T>());

        if (!strncmp(key, get_key_ptr(o), get_key_size<T>())) {
            return true;
        }
    }

    return false;
}

template<typename T>
bool
ModelStorage<T>::search(const char* key, T &t)
{
    for (ObjIndex i = 0; isOkIdx(i); ++i) {
        if (isFreeAccount(i)) {
            continue;
        }

        bs_.read(getKeyAddr(i), reinterpret_cast<uint8_t*>(get_key_ptr(t)), get_key_size<T>());

        if (!strncmp(key, get_key_ptr(t), get_key_size<T>())) {
            bs_.read(idx2addr(i), t);
            return true;
        }
    }

    return false;
}

template<typename T>
bool
ModelStorage<T>::isOkIdx(const ObjIndex idx) const
{
    size_t startAddr = idx2addr(idx);
    return bs_.isAddrOk(startAddr) && (bs_.isAddrOk(startAddr + sizeof(T) - sizeof(ObjInStorage::commitFlag_)));
}

template<typename T>
size_t
ModelStorage<T>::getKeyAddr(const ObjIndex idx)
{
    return idx2addr(idx) + get_key_offset<T>();
}

template<typename T>
bool
ModelStorage<T>::empty() const
{
    for (ObjIndex idx = 0; isOkIdx(idx); ++idx) {
        if (!isFreeAccount(idx)) {
            return false;
        }
    }
    return true;
}

template<typename T>
typename ModelStorage<T>::ObjIndex
ModelStorage<T>::count() const
{
    ObjIndex c = 0;
    for (ObjIndex idx = 0; isOkIdx(idx); ++idx) {
        if (!isFreeAccount(idx)) {
            ++c;
        }
    }
    return c;
}

template<typename T>
typename ModelStorage<T>::ObjIndex
ModelStorage<T>::maxIdx() const
{
    size_t maxAvailableSlots = (bs_.maxAddr() - bs_.minAddr()) / sizeof(ObjInStorage);
    return maxAvailableSlots > 0 ? maxAvailableSlots - 1 : 0;
}

template<typename T>
bool
ModelStorage<T>::isFreeAccount(const ObjIndex idx) const
{
    return static_cast<uint8_t>(ObjInStorage::Committment::free) == bs_.read(idx2addr(idx) + offsetof(ObjInStorage, commitFlag_));
}

template<typename T>
bool
ModelStorage<T>::get(const ObjIndex idx, T &o) const
{
    bs_.read(idx2addr(idx), o);
    return !isFreeAccount(idx);
}

template<typename T>
bool
ModelStorage<T>::getNext(const ObjIndex from, T &o, ObjIndex &idx) const
{
    for (ObjIndex i = from + 1; isOkIdx(i); ++i) {
        if (!isFreeAccount(i)) {
            idx = i;
            get(i, o);
            return true;
        }
    }
    return false;
}

template<typename T>
bool
ModelStorage<T>::getPrev(const ObjIndex from, T &o, ObjIndex &idx) const
{
    for (ObjIndex i = from; i > 0; --i) {  // if set "i >= 0" as stop condition then index 0 will be missed
        ObjIndex pos = i > 0 ? i - 1 : 0;

        if (!isFreeAccount(pos)) {
            idx = pos;
            get(pos, o);
            return true;
        }
    }
    return false;
}

template<typename T>
bool
ModelStorage<T>::add(const T &o)
{
    ObjIndex idx = 0;
    if (!getFreeObjectIndex(idx)) {
        return false;
    }

    bs_.write(idx2addr(idx), o);
    bs_.write(idx2addr(idx) + offsetof(ObjInStorage, commitFlag_), 
              static_cast<uint8_t>(ObjInStorage::Committment::comitted));
    return true;
}

template<typename T>
bool
ModelStorage<T>::getFreeObjectIndex(ObjIndex &idx)
{
    for (ObjIndex i = 0; isOkIdx(i); ++i) {
        if (isFreeAccount(i)) {
            idx = i;
            return true;
        }
    }
    return false;
}

template<typename T>
bool
ModelStorage<T>::del(const char* key)
{
    T o;

    for (ObjIndex i = 0; isOkIdx(i); ++i) {
        if (isFreeAccount(i)) {
            continue;
        }

        bs_.read(getKeyAddr(i), reinterpret_cast<uint8_t*>(get_key_ptr(o)), get_key_size<T>());

        if (!strncmp(key, get_key_ptr(o), get_key_size<T>())) {
            // TODO: make zeroing Account slot for more secure
            // TODO: add count of writes to slot to stop use it after EEPROM max read/write operations limit
            bs_.write(idx2addr(i) + offsetof(ObjInStorage, commitFlag_), 
                      static_cast<uint8_t>(ObjInStorage::Committment::free));
            return true;
        }
    }

    return false;
}

template<typename T>
size_t
ModelStorage<T>::idx2addr(const ObjIndex idx) const
{
    return bs_.minAddr() + idx * sizeof(ObjInStorage);
}

template<typename T>
void
ModelStorage<T>::factoryReset()
{
    bs_.factoryReset();
}

template<typename T>
ModelIterator<T>
ModelStorage<T>::cbegin() const
{
    // special case when storage is empty: need to return the same as end()
    if (empty()) {
        return cend();
    }
    return ModelIterator<T>(*this, ModelIterator<T>::FIRST);
}

template<typename T>
ModelIterator<T>
ModelStorage<T>::cend() const
{
    return ModelIterator<T>(*this, ModelIterator<T>::END);
}
