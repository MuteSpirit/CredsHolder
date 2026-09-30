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

// !!! Do not change sequence of next includes to avoid compilation error like
//   ‘ModelIterator’ does not name a type [-Wtemplate-body
#include "storage.hpp"
#include "iterator.hpp"

#include "account.hpp"
#include "../memory_block_storage.hpp"

// Must be included as the last header to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

// TODO: figure out why test with fixture crash with segfault
class ModelIteratorTest : public aunit::TestOnce
{
protected:
    void setup() override
    {
        TestOnce::setup();

        bs_.factoryReset();
        //
        // assertTrue(ms_.add(acc0));
        // assertTrue(ms_.add(acc1));
        // assertTrue(ms_.add(acc2));
    }

protected:
    // Account acc0 {.name = "n0", .username = "u0", .password = "p0"};
    // Account acc1 {.name = "n1", .username = "u1", .password = "p1"};
    // Account acc2 {.name = "n2", .username = "u2", .password = "p2"};
    //
    MemoryBlockStorage<1024, 64> bs_;
    ModelStorage<Account> ms_ {bs_};
};

testF(ModelIteratorTest, ctor_empty_iterator)
{
    Account emptyAcc;
    acc_ctor(emptyAcc);

    ModelIterator<Account> it(ms_);
    
    assertTrue(*it == emptyAcc);
};

#endif // EPOXY_DUINO
