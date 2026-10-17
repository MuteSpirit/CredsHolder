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

#include "accounts_menu.hpp"

#include <Embedded_Template_Library.h>
#include <etl/array.h>

// Avoid double definition:
//   fatfs.h includes ff.h which includes ff-inc.h which again include ff.h
//   FATFS_HEADER_ONLY_IMPL definition allows avoid include ff-inc.h by ff.h
#define FATFS_HEADER_ONLY_IMPL
#include <fatfs.h>

#include "sqlite.hpp"
#include "sqlite3_vfs_over_fatfs.hpp"
#include "model/sql/account_storage.hpp"

#include "device_inputs.hpp"
#include "settings.hpp"
#include "memory_block_storage.hpp"

#include "model/account.hpp"
#include "model/blob/account_storage.hpp"

#include "t/in_memory_oled.hpp"
#include "t/imitated_user_unputs.hpp"

// Must be included as the last one to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

using namespace creds_holder;

////////////////////////////////////////////////////////////////////////////////
class AccountsMenuWithAccountBlobStorageTest : public aunit::TestOnce
{
public:
    virtual void setup() override
    {
        TestOnce::setup();

        memset(buf, 0, sz);
    }

protected:
    OledInMem<16, 4> oled;
    ImitatedUserInputs userInputs;
    MemoryBlockStorage<4096, 64> bs_;
    AccountBlobStorage ms {bs_};

    constexpr static uint8_t sz = 16;
    char buf[sz];
};

testF(AccountsMenuWithAccountBlobStorageTest, ctor)
{
    AccountsMenu<AccountBlobStorage> menu(oled, userInputs, ms);
};

testF(AccountsMenuWithAccountBlobStorageTest, draw_empty_account_storage)
{
    AccountsMenu<AccountBlobStorage> menu(oled, userInputs, ms);

    menu.draw();

    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("", buf);
}

testF(AccountsMenuWithAccountBlobStorageTest, draw_single_account)
{
    Account acc {0, "acc0", "url", "login", "passwd"};
    assertTrue(ms.add(acc));

    AccountsMenu<AccountBlobStorage> menu(oled, userInputs, ms);

    menu.draw();

    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("acc0", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("", buf);
}

testF(AccountsMenuWithAccountBlobStorageTest, draw_the_same_accounts_as_visible)
{
    etl::array<Account, 3> aa {{
        {0, "acc0", "url0", "login0", "passwd0"},
        {1, "acc1", "url1", "login1", "passwd1"},
        {2, "acc2", "url2", "login2", "passwd2"},
    }};

    for (auto acc : aa) {
        assertTrue(ms.add(acc));
    }

    AccountsMenu<AccountBlobStorage> menu(oled, userInputs, ms);

    assertTrue(aa[0] == menu.selected());

    oled.clear();
    menu.draw();

    memset(buf, 0, sz);
    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("acc0", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("acc1", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("acc2", buf);

    menu.activate();

    userInputs.tilt(DeviceInputs::UserAction::down); // acc0 ->acc1
    assertTrue(aa[1] == menu.selected());

    userInputs.tilt(DeviceInputs::UserAction::down); // acc1 ->acc2
    assertTrue(aa[2] == menu.selected());

    userInputs.tilt(DeviceInputs::UserAction::down); // acc2 ->acc0
    assertTrue(aa[0] == menu.selected());

    oled.clear();
    menu.draw();

    memset(buf, 0, sz);
    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("acc0", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("acc1", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("acc2", buf);
}

testF(AccountsMenuWithAccountBlobStorageTest, draw_more_accounts_then_visible)
{
    etl::array<Account, 4> aa {{
        {0, "acc0", "url0", "login0", "passwd0"},
        {1, "acc1", "url1", "login1", "passwd1"},
        {2, "acc2", "url2", "login2", "passwd2"},
        {3, "acc3", "url3", "login3", "passwd3"},
    }};

    for (auto acc : aa) {
        assertTrue(ms.add(acc));
    }

    AccountsMenu<AccountBlobStorage> menu(oled, userInputs, ms);
    menu.activate();

    assertTrue(aa[0] == menu.selected());
    menu.draw();

    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("acc0", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("acc1", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("acc2", buf);

    userInputs.tilt(DeviceInputs::UserAction::down); // acc0 ->acc1
    assertTrue(aa[1] == menu.selected());

    userInputs.tilt(DeviceInputs::UserAction::down); // acc1 ->acc2
    assertTrue(aa[2] == menu.selected());

    userInputs.tilt(DeviceInputs::UserAction::down); // acc2 ->acc3
    assertTrue(aa[3] == menu.selected());

    menu.draw();

    memset(buf, 0, sz);
    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("acc1", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("acc2", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("acc3", buf);
}

////////////////////////////////////////////////////////////////////////////////
class AccountsMenuWithAccountSqlStorageTest : public aunit::TestOnce
{
    using Base = aunit::TestOnce;

protected:
    void setup() override
    {
        Base::setup();

        memset(work, 0, sizeof(work));
        //
        // Create MBR ...
        LBA_t plist[] = {100, 0};
        assertTrue(fatfs::FR_OK == fs.f_fdisk(0, plist, work));
        // ... and format FAT
        assertTrue(fatfs::FR_OK == fs.f_mkfs("0:", nullptr, work, sizeof(work)));

        assertTrue(fatfs::FR_OK == fatfs::vfs::register_mount(nullptr, &fs, mountPoint));

        assertTrue(SQLITE_OK == sqlite3_initialize());

        assertTrue(SQLITE_OK == sqlite3_open_v2("/rom0/test.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, fatfs::vfs::name()));
        assertNotEqual(nullptr, db);

        ms.init(db);
        assertTrue(ms.initSchema());
    }

    void teardown() override
    {
        sqlite3_close_v2(db);
        db = nullptr;

        sqlite3_shutdown();

        fatfs::vfs::unregister_mount(nullptr, mountPoint);

        fs.f_unmount("");

        memset(work, 0, sizeof(work));

        Base::teardown();
    }

protected:
    fatfs::RamIO<1024 /*sectors*/> drv {512 /*bytes per sector*/};
    uint8_t work[FF_MAX_SS];
    fatfs::FatFs fs {drv};
    sqlite3 *db {nullptr};
    static constexpr const char* mountPoint = "/rom0";
    static constexpr const char* dbPath = "/rom0/test.db";

    OledInMem<16, 4> oled;
    ImitatedUserInputs userInputs;
    AccountSqlStorage ms;

    constexpr static uint8_t sz = 16;
    char buf[sz];
};

testF(AccountsMenuWithAccountSqlStorageTest, draw_more_accounts_then_visible)
{
    etl::array<Account, 4> aa {{
        {0, "acc0", "url0", "login0", "passwd0"},
        {1, "acc1", "url1", "login1", "passwd1"},
        {2, "acc2", "url2", "login2", "passwd2"},
        {3, "acc3", "url3", "login3", "passwd3"},
    }};

    for (auto& acc : aa) {
        assertTrue(ms.add(acc));
    }

    AccountsMenu<AccountSqlStorage> menu(oled, userInputs, ms);
    menu.activate();

    assertTrue(aa[0] == menu.selected());
    menu.draw();

    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("acc0", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("acc1", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("acc2", buf);

    userInputs.tilt(DeviceInputs::UserAction::down); // acc0 ->acc1
    assertTrue(aa[1] == menu.selected());

    userInputs.tilt(DeviceInputs::UserAction::down); // acc1 ->acc2
    assertTrue(aa[2] == menu.selected());

    userInputs.tilt(DeviceInputs::UserAction::down); // acc2 ->acc3
    assertTrue(aa[3] == menu.selected());

    menu.draw();

    memset(buf, 0, sz);
    oled.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("acc1", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("acc2", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("acc3", buf);
}

#endif // defined(EPOXY_DUINO)
