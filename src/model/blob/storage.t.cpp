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
#include "storage.hpp"
#include "iterator.hpp"

#include <string.h>
#include "../account.hpp"
#include "account_storage.hpp"
#include "../../memory_block_storage.hpp"

// Must be included as the last header to avoid troubles with macro "test"
// when such word is used in headers above
#include "../../aunit.hpp"

using namespace creds_holder;

test(model_storage_ctor)
{
    MemoryBlockStorage<1024, 64> bs;
    AccountBlobStorage model(bs);
};

test(model_storage_is_persist)
{
    MemoryBlockStorage<1024, 64> bs;

    Account acc {0, "n", "url", "u", "p"};
    // a'la first device with block storage initialization
    {
        AccountBlobStorage m(bs);

        assertTrue(m.add(acc));
        assertEqual(1, m.count());
    }
    // a'la second/one-of-next device boot with data in ext EEPROM
    {
        AccountBlobStorage m(bs);

        assertEqual(1, m.count());
        assertTrue(m.isExist(acc.name));
    }
};



////////////////////////////////////////////////////////////////////////////////
class ModelStorageTest : public aunit::TestOnce
{
protected:
    void setup() override
    {
        TestOnce::setup();

        bs_.factoryReset();
    }

    MemoryBlockStorage<4096, 64> bs_;
    AccountBlobStorage ms_ {bs_};
};

testF(ModelStorageTest, add)
{
    Account acc {0, "n", "url", "u", "p"};

    assertEqual(0, ms_.count());
    assertFalse(ms_.isExist(acc.name));

    assertTrue(ms_.add(acc));

    assertEqual(1, ms_.count());
    assertTrue(ms_.isExist(acc.name));
};

testF(ModelStorageTest, get)
{
    Account acc0 {0, "n", "url", "u", "p"};

    assertTrue(ms_.add(acc0));

    Account acc1;
    assertTrue(ms_.get(0, acc1));

    assertStringCaseEqual(acc0.name, acc1.name);
};

testF(ModelStorageTest, get_next_prev)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    Account acc1 {1, "n1", "url", "u1", "p1"};

    assertTrue(ms_.add(acc0));
    assertTrue(ms_.add(acc1));

    Account acc;

    typename AccountBlobStorage::ObjIndex idx = 0;
    //
    // Next
    assertTrue(ms_.getNext(0, acc, idx));

    assertEqual(1, idx);
    assertStringCaseEqual(acc1.name, acc.name);
    //
    // Prev
    acc.reset();

    assertTrue(ms_.getPrev(idx, acc, idx));

    assertEqual(0, idx);
    assertStringCaseEqual(acc0.name, acc.name);
};

testF(ModelStorageTest, del_one_object)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    assertTrue(ms_.add(acc0));

    assertTrue(ms_.del(acc0.name));

    assertEqual(0, ms_.count());
};

testF(ModelStorageTest, del_three_objects_fifo)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    Account acc1 {1, "n1", "url", "u1", "p1"};
    Account acc2 {2, "n2", "url", "u2", "p2"};

    assertTrue(ms_.add(acc0));
    assertTrue(ms_.add(acc1));
    assertTrue(ms_.add(acc2));

    assertEqual(3, ms_.count());

    assertTrue(ms_.del(acc0.name));
    assertEqual(2, ms_.count());

    assertTrue(ms_.del(acc1.name));
    assertEqual(1, ms_.count());

    assertTrue(ms_.del(acc2.name));
    assertEqual(0, ms_.count());
};

testF(ModelStorageTest, del_three_objects_lifo)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    Account acc1 {1, "n1", "url", "u1", "p1"};
    Account acc2 {2, "n2", "url", "u2", "p2"};

    assertTrue(ms_.add(acc0));
    assertTrue(ms_.add(acc1));
    assertTrue(ms_.add(acc2));

    assertTrue(ms_.del(acc2.name));
    assertEqual(2, ms_.count());

    assertTrue(ms_.del(acc1.name));
    assertEqual(1, ms_.count());

    assertTrue(ms_.del(acc0.name));
    assertEqual(0, ms_.count());
};

testF(ModelStorageTest, get_next_prev_over_free_spot)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    Account acc1 {1, "n1", "url", "u1", "p1"};
    Account acc2 {2, "n2", "url", "u2", "p2"};

    assertTrue(ms_.add(acc0));
    assertTrue(ms_.add(acc1));
    assertTrue(ms_.add(acc2));

    assertTrue(ms_.del(acc1.name));

    Account acc;

    typename AccountBlobStorage::ObjIndex idx = 0;
    //
    // Next
    assertTrue(ms_.getNext(0, acc, idx));

    assertEqual(2, idx);
    assertStringCaseEqual(acc2.name, acc.name);
    //
    // Prev
    acc.reset();

    assertTrue(ms_.getPrev(idx, acc, idx));

    assertEqual(0, idx);
    assertStringCaseEqual(acc0.name, acc.name);
};

testF(ModelStorageTest, add_del_and_add_to_fill_free_spot)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    Account acc1 {1, "n1", "url", "u1", "p1"};
    Account acc2 {2, "n2", "url", "u2", "p2"};

    assertTrue(ms_.add(acc0));
    assertTrue(ms_.add(acc1));
    assertTrue(ms_.add(acc2));

    assertTrue(ms_.del(acc1.name));
    assertEqual(2, ms_.count());

    Account acc3 {0, "n3", "url", "u3", "p3"};
    assertTrue(ms_.add(acc3));
    assertEqual(3, ms_.count());

    Account acc;

    typename AccountBlobStorage::ObjIndex idx = 0;
    //
    // Next
    assertTrue(ms_.getNext(0, acc, idx));

    assertEqual(1, idx);
    assertStringCaseEqual(acc3.name, acc.name);
};

testF(ModelStorageTest, cbegin_for_empty_storage)
{
    ModelIterator<Account> it(ms_.cbegin());

    Account emptyAcc;

    assertTrue(emptyAcc == *it);
};

testF(ModelStorageTest, cend_for_empty_storage)
{
    ModelIterator<Account> it(ms_.cend());

    Account emptyAcc;

    assertTrue(emptyAcc == *it);
};

testF(ModelStorageTest, cbegin_equal_to_cend_for_empty_storage)
{
    assertTrue(ms_.cbegin() == ms_.cend());
};

testF(ModelStorageTest, cbegin_not_equal_to_cend_for_non_empty_storage)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    assertTrue(ms_.add(acc0));
    ModelIterator<Account> it = ms_.cbegin();
    assertTrue(it != ms_.cend());
    assertTrue(acc0 == *it);
};

testF(ModelStorageTest, prev_cend_for_empty_storage)
{
    ModelIterator<Account> it = ms_.cend();
    assertTrue(ms_.cend() == --it);
}

testF(ModelStorageTest, prev_cbegin_for_non_empty_storage)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    assertTrue(ms_.add(acc0));

    ModelIterator<Account> it = ms_.cbegin();
    assertTrue(acc0 == *(--it));
}

testF(ModelStorageTest, prev_cend_for_non_empty_storage)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    assertTrue(ms_.add(acc0));

    ModelIterator<Account> it = ms_.cend();
    assertTrue(acc0 == *(--it));
}

testF(ModelStorageTest, iterate_from_cbegin_for_empty_storage)
{
    ModelIterator<Account> it = ms_.cbegin();
    assertTrue(ms_.cend() == ++it);
}

testF(ModelStorageTest, iterate_from_cbegin_to_cend_for_non_empty_storage)
{
    Account acc0 {0, "n0", "url", "u0", "p0"};
    assertTrue(ms_.add(acc0));
    ModelIterator<Account> it = ms_.cbegin();
    assertTrue(ms_.cend() == ++it);
}

#endif // EPOXY_DUINO
