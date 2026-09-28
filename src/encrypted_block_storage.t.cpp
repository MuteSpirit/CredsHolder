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
#include "encrypted_block_storage.cpp"
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

#endif // EPOXY_DUINO
