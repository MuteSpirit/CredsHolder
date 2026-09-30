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
#include "encrypted_block_storage.hpp"

#include <inttypes.h>
#include <string.h>
#include "model/account.hpp"
#include "model/storage.hpp"
#include "memory_block_storage.hpp"

// Must be included as the last header to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

// AES-256 requires a key that is exactly 32 bytes (256 bits).
constexpr const uint8_t encStoreKey[] = "32-length-password-0123456789012";
constexpr size_t encStoreKeyLen = (sizeof("32-length-password-0123456789012") / sizeof(encStoreKey[0])) - 1;

test(encrypted_store_ctor)
{
    MemoryBlockStorage<128, 16> mbs;
    EncryptedBlockStorage ebs(mbs);
};

test(encrypted_store_init)
{
    MemoryBlockStorage<128, 16> mbs;
    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));
};

test(encrypted_store_negative_work_without_init)
{
    MemoryBlockStorage<128, 16> mbs;
    EncryptedBlockStorage ebs(mbs);
    ebs.factoryReset();

    ebs.write(0x0, 1);
    // Let's decide to do no one write/read operations without preliminary 'init(...)' call

    assertEqual(0, mbs.read(0x0));
    assertEqual(0, ebs.read(0x0));
}

test(encrypted_store_factory_reset)
{
    MemoryBlockStorage<128, 16> mbs;
    EncryptedBlockStorage ebs(mbs);
    ebs.init(encStoreKey, encStoreKeyLen);
    //
    // Check that underline block storage will be reset
    mbs.write(0x0, 1);
    ebs.factoryReset();

    assertEqual(0, mbs.read(0x0));
};

test(encrypted_store_caching_writes)
{
    MemoryBlockStorage<128, 16> mbs;
    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));
    ebs.factoryReset();

    // write flag value into underline block storage to not depend on default value
    // after factory reset
    for (size_t addr = 0; addr < AES_BLOCK_SIZE; ++addr) {
        mbs.write(addr, 0xFF);
    }

    // AES block size if 16, so up to 15 sequential bytes write should be cached
    for (size_t addr = 0; addr < AES_BLOCK_SIZE; ++addr) {
        ebs.write(addr, 1);
        assertEqual(0xFF, mbs.read(addr));
    }

    // after try to write byte not into current block EncryptedBlockStorage should flush
    // current one into underline block storage before start handle our request
    ebs.write(AES_BLOCK_SIZE, 0xFF);
    for (size_t addr = 0; addr < AES_BLOCK_SIZE; ++addr) {
        uint8_t b = mbs.read(addr);
        assertNotEqual(0xFF, b);
    }
};

test(encrypted_store_write_block_by_one_shot)
{
    MemoryBlockStorage<128, 16> mbs;
    EncryptedBlockStorage ebs(mbs);
    ebs.init(encStoreKey, encStoreKeyLen);
    ebs.factoryReset();

    // write flag value into underline block storage to not depend on default value
    // after factory reset
    for (size_t addr = 0; addr < AES_BLOCK_SIZE; ++addr) {
        mbs.write(addr, 0xFF);
    }
#define FIRST_BLOCK_TEXT  "0123456789ABCDEF"
#define SECOND_BLOCK_TEXT "fedcba9076543210"
    {
        char buf[AES_BLOCK_SIZE + 1] = FIRST_BLOCK_TEXT;
        ebs.write(0x0, reinterpret_cast<uint8_t*>(buf), AES_BLOCK_SIZE);
    }
    { // trigger EncryptedBlockStorage flush by writing next block
        char buf[AES_BLOCK_SIZE + 1] = SECOND_BLOCK_TEXT;
        ebs.write(AES_BLOCK_SIZE, reinterpret_cast<uint8_t*>(buf), AES_BLOCK_SIZE);
    }
    {
        char buf[AES_BLOCK_SIZE + 1] = {0};
        ebs.read(0x0, reinterpret_cast<uint8_t*>(buf), AES_BLOCK_SIZE);
        assertStringCaseEqual(FIRST_BLOCK_TEXT, buf);
    }
    { // trigger EncryptedBlockStorage flush by writing next block
        char buf[AES_BLOCK_SIZE + 1] = {0};
        ebs.read(AES_BLOCK_SIZE, reinterpret_cast<uint8_t*>(buf), AES_BLOCK_SIZE);
        assertStringCaseEqual(SECOND_BLOCK_TEXT, buf);
    }
#undef FIRST_BLOCK_TEXT
#undef SECOND_BLOCK_TEXT
}

test(encrypted_model_storage_ctor)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> model(ebs);
};

test(encrypted_model_storage_add)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc {.name = "n", .username = "u", .password = "p"};

    assertEqual(0, m.count());
    assertFalse(m.isExist(acc.name));

    assertTrue(m.add(acc));

    assertEqual(1, m.count());
    assertTrue(m.isExist(acc.name));
};

test(encrypted_model_storage_get)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc0 {.name = "n", .username = "u", .password = "p"};

    assertTrue(m.add(acc0));

    Account acc1;
    assertTrue(m.get(0, acc1));

    assertStringCaseEqual(acc0.name, acc1.name);
};

test(encrypted_model_storage_get_next_prev)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc0 {.name = "n0", .username = "u0", .password = "p0"};
    Account acc1 {.name = "n1", .username = "u1", .password = "p1"};

    assertTrue(m.add(acc0));
    assertTrue(m.add(acc1));

    Account acc;
    memset(&acc, 0, sizeof(acc));

    typename ModelStorage<Account>::ObjIndex idx = 0;
    //
    // Next
    assertTrue(m.getNext(0, acc, idx));

    assertEqual(1, idx);
    assertStringCaseEqual(acc1.name, acc.name);
    //
    // Prev
    memset(&acc, 0, sizeof(acc));

    assertTrue(m.getPrev(idx, acc, idx));

    assertEqual(0, idx);
    assertStringCaseEqual(acc0.name, acc.name);
};

test(encrypted_model_storage_del_one_object)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc0 {.name = "n0", .username = "u0", .password = "p0"};
    assertTrue(m.add(acc0));

    assertTrue(m.del(acc0.name));

    assertEqual(0, m.count());
};

test(encrypted_model_storage_del_three_objects_fifo)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc0 {.name = "n0", .username = "u0", .password = "p0"};
    Account acc1 {.name = "n1", .username = "u1", .password = "p1"};
    Account acc2 {.name = "n2", .username = "u2", .password = "p2"};

    assertTrue(m.add(acc0));
    assertTrue(m.add(acc1));
    assertTrue(m.add(acc2));

    assertEqual(3, m.count());

    assertTrue(m.del(acc0.name));
    assertEqual(2, m.count());

    assertTrue(m.del(acc1.name));
    assertEqual(1, m.count());

    assertTrue(m.del(acc2.name));
    assertEqual(0, m.count());
};

test(encrypted_model_storage_del_three_objects_lifo)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc0 {.name = "n0", .username = "u0", .password = "p0"};
    Account acc1 {.name = "n1", .username = "u1", .password = "p1"};
    Account acc2 {.name = "n2", .username = "u2", .password = "p2"};

    assertTrue(m.add(acc0));
    assertTrue(m.add(acc1));
    assertTrue(m.add(acc2));

    assertTrue(m.del(acc2.name));
    assertEqual(2, m.count());

    assertTrue(m.del(acc1.name));
    assertEqual(1, m.count());

    assertTrue(m.del(acc0.name));
    assertEqual(0, m.count());
};

test(encrypted_model_storage_get_next_prev_over_free_spot)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc0 {.name = "n0", .username = "u0", .password = "p0"};
    Account acc1 {.name = "n1", .username = "u1", .password = "p1"};
    Account acc2 {.name = "n2", .username = "u2", .password = "p2"};

    assertTrue(m.add(acc0));
    assertTrue(m.add(acc1));
    assertTrue(m.add(acc2));

    assertTrue(m.del(acc1.name));

    Account acc;
    memset(&acc, 0, sizeof(acc));

    typename ModelStorage<Account>::ObjIndex idx = 0;
    //
    // Next
    assertTrue(m.getNext(0, acc, idx));

    assertEqual(2, idx);
    assertStringCaseEqual(acc2.name, acc.name);
    //
    // Prev
    memset(&acc, 0, sizeof(acc));

    assertTrue(m.getPrev(idx, acc, idx));

    assertEqual(0, idx);
    assertStringCaseEqual(acc0.name, acc.name);
};

test(encrypted_model_storage_add_del_and_add_to_fill_free_spot)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    ModelStorage<Account> m(ebs);

    Account acc0 {.name = "n0", .username = "u0", .password = "p0"};
    Account acc1 {.name = "n1", .username = "u1", .password = "p1"};
    Account acc2 {.name = "n2", .username = "u2", .password = "p2"};

    assertTrue(m.add(acc0));
    assertTrue(m.add(acc1));
    assertTrue(m.add(acc2));

    assertTrue(m.del(acc1.name));
    assertEqual(2, m.count());

    Account acc3 {.name = "n3", .username = "u3", .password = "p3"};
    assertTrue(m.add(acc3));
    assertEqual(3, m.count());

    Account acc;
    memset(&acc, 0, sizeof(acc));

    typename ModelStorage<Account>::ObjIndex idx = 0;
    //
    // Next
    assertTrue(m.getNext(0, acc, idx));

    assertEqual(1, idx);
    assertStringCaseEqual(acc3.name, acc.name);
};

test(encrypted_model_storage_is_persist)
{
    MemoryBlockStorage<1024, 64> mbs;
    mbs.factoryReset();

    EncryptedBlockStorage ebs(mbs);
    assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

    Account acc {.name = "n", .username = "u", .password = "p"};
    // a'la first device with block storage initialization
    {
        ModelStorage<Account> m(ebs);

        assertTrue(m.add(acc));
        assertEqual(1, m.count());
    }
    // a'la second/one-of-next device boot with data in ext EEPROM
    {
        ModelStorage<Account> m(ebs);

        assertEqual(1, m.count());
        assertTrue(m.isExist(acc.name));
    }
}

#endif // EPOXY_DUINO
