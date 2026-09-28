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
#if defined(EPOXY_DUINO)
#include "memory_block_storage.hpp"
#include <stdio.h>

// Must be included as the last header to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

class MemoryBlockStorageTestHelper
{
public:
    template<size_t fullSizeBytes, size_t pageSizeBytes>
    static uint8_t *get(MemoryBlockStorage<fullSizeBytes, pageSizeBytes> &store) {
        return store.store_;
    }

};

using TH = MemoryBlockStorageTestHelper;

test(mem_blk_store_ctor)
{
    MemoryBlockStorage<128,16> store;
    uint8_t *_store = TH::get(store);
    assertEqual(0, _store[0]);
    assertEqual(0, store.read(0));
};

test(mem_blk_store_write_read_byte)
{
    MemoryBlockStorage<0x80,0x10> store;
    uint8_t *_store = TH::get(store);

    store.write(0, 0x12);
    assertEqual(0x12, store.read(0));
    assertEqual(0x12, _store[0]);

    store.write(0x40, 0x12);
    assertEqual(0x12, store.read(0x40));
    assertEqual(0x12, _store[0x40]);
};

test(mem_blk_store_write_read_array)
{
    MemoryBlockStorage<0x80,0x10> store;
    uint8_t *_store = TH::get(store);

    uint8_t buf[0x2] = {0x34, 0x21};

    store.write(0, buf, 0x2);
    assertEqual(0x34, store.read(0));
    assertEqual(0x34, _store[0]);

    store.write(0x40, buf, 0x2);
    assertEqual(0x34, store.read(0x40));
    assertEqual(0x34, _store[0x40]);
};

test(mem_blk_store_not_allow_harm_reset_flag)
{
    const uint16_t sz = 0x80;

    MemoryBlockStorage<sz, 0x10> store;
    uint8_t *_store = TH::get(store);

    assertEqual(0xff, _store[sz - 1]);

    store.write(sz - 1, 0x01);
    assertEqual(0xff, _store[sz - 1]);
};

test(mem_blk_store_factory_reset)
{
    MemoryBlockStorage<0x80,0x10> store;

    store.write(0,        0x1);
    store.write(0x40,     0x2);
    store.write(0x80 - 2, 0x3);

    store.factoryReset();

    assertEqual(0, store.read(0));
    assertEqual(0, store.read(0x40));
    assertEqual(0, store.read(0x80 - 2));
};
#endif // EPOXY_DUINO
