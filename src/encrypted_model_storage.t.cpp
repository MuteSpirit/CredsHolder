#if defined(EPOXY_DUINO)
#include <inttypes.h>
#include <string.h>
#include <AUnitVerbose.h>

#include "model.hpp"
#include "model_storage.hpp"
#include "memory_block_storage.hpp"
#include "encrypted_block_storage.hpp"

// AES-256 requires a key that is exactly 32 bytes (256 bits).
constexpr const uint8_t encStoreKey[] = "32-length-password-0123456789012";
constexpr size_t encStoreKeyLen = (sizeof("32-length-password-0123456789012") / sizeof(encStoreKey[0])) - 1;

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

    uint16_t idx = 0;
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

    uint16_t idx = 0;
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

    uint16_t idx = 0;
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
