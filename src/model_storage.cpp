#include "model_storage.hpp"
#include "model.hpp"
#include "block_storage.hpp"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>


// Let's instantiate for Account to keep control on ModelStorage class usage
template class ModelStorage<Account>;

template<typename Object>
bool
ModelStorage<Object>::isExist(const char* key)
{
    Object o;  
    for (ObjIndex i = 0; isOkIdx(i); ++i) {
        if (isFreeAccount(i)) {
            continue;
        }

        bs_.read(getKeyAddr(i), reinterpret_cast<uint8_t*>(get_key_ptr(o)), get_key_size<Object>());

        if (!strncmp(key, get_key_ptr(o), get_key_size<Object>())) {
            return true;
        }
    }

    return false;
}

template<typename Object>
bool
ModelStorage<Object>::isOkIdx(const ObjIndex idx) const
{
    size_t startAddr = idx2addr(idx);
    return bs_.isAddrOk(startAddr) && (bs_.isAddrOk(startAddr + sizeof(Object) - sizeof(ObjInStorage::commitFlag_)));
}

template<typename Object>
size_t
ModelStorage<Object>::getKeyAddr(const ObjIndex idx)
{
    return idx2addr(idx) + get_key_offset<Object>();
}

template<typename Object>
typename ModelStorage<Object>::ObjIndex
ModelStorage<Object>::count() const
{
    ObjIndex c = 0;
    for (ObjIndex idx = 0; isOkIdx(idx); ++idx) {
        if (!isFreeAccount(idx)) {
            ++c;
        }
    }
    return c;
}

template<typename Object>
typename ModelStorage<Object>::ObjIndex
ModelStorage<Object>::maxIdx() const
{
    size_t maxAvailableSlots = (bs_.maxAddr() - bs_.minAddr()) / sizeof(ObjInStorage);
    return maxAvailableSlots > 0 ? maxAvailableSlots - 1 : 0;
}

template<typename Object>
bool
ModelStorage<Object>::isFreeAccount(const ObjIndex idx) const
{
    return static_cast<uint8_t>(ObjInStorage::Committment::free) == bs_.read(idx2addr(idx) + offsetof(ObjInStorage, commitFlag_));
}

template<typename Object>
bool
ModelStorage<Object>::get(const ObjIndex idx, Object &o)
{
    bs_.read(idx2addr(idx), o);
    return true;
}

template<typename Object>
bool
ModelStorage<Object>::getNext(const ObjIndex from, Object &o, ObjIndex &idx)
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

template<typename Object>
bool
ModelStorage<Object>::getPrev(const ObjIndex from, Object &o, ObjIndex &idx)
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

template<typename Object>
bool
ModelStorage<Object>::add(const Object &o)
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

template<typename Object>
bool
ModelStorage<Object>::getFreeObjectIndex(ObjIndex &idx)
{
    for (ObjIndex i = 0; isOkIdx(i); ++i) {
        if (isFreeAccount(i)) {
            idx = i;
            return true;
        }
    }
    return false;
}

template<typename Object>
bool
ModelStorage<Object>::del(const char* key)
{
    Object o;

    for (ObjIndex i = 0; isOkIdx(i); ++i) {
        if (isFreeAccount(i)) {
            continue;
        }

        bs_.read(getKeyAddr(i), reinterpret_cast<uint8_t*>(get_key_ptr(o)), get_key_size<Object>());

        if (!strncmp(key, get_key_ptr(o), get_key_size<Object>())) {
            // TODO: make zeroing Account slot for more secure
            // TODO: add count of writes to slot to stop use it after EEPROM max read/write operations limit
            bs_.write(idx2addr(i) + offsetof(ObjInStorage, commitFlag_), 
                      static_cast<uint8_t>(ObjInStorage::Committment::free));
            return true;
        }
    }

    return false;
}

template<typename Object>
size_t
ModelStorage<Object>::idx2addr(const ObjIndex idx) const
{
    return bs_.minAddr() + idx * sizeof(ObjInStorage);
}

template<typename Object>
void
ModelStorage<Object>::factoryReset()
{
    bs_.factoryReset();
}
