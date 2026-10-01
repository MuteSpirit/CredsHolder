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

#include <array>

#include "device_inputs.hpp"
#include "settings.hpp"
#include "memory_block_storage.hpp"

#include "model/account.hpp"
#include "model/storage.hpp"

#include "t/in_memory_oled.hpp"
#include "t/imitated_user_unputs.hpp"

// Must be included as the last one to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

class AccountsMenuTest : public aunit::TestOnce
{
public:
    virtual void setup() override
    {
        TestOnce::setup();

        memset(buf, 0, sz);
    }

protected:
    OledInMem<16, 4> oled_;
    ImitatedUserInputs userInputs_;
    MemoryBlockStorage<1024, 64> bs_;
    ModelStorage<Account> ms_ {bs_};

    constexpr static uint8_t sz = 16;
    char buf[sz];
};

testF(AccountsMenuTest, ctor)
{
    AccountsMenu menu(oled_, userInputs_, ms_);
};

testF(AccountsMenuTest, draw_empty_account_storage)
{
    AccountsMenu menu(oled_, userInputs_, ms_);

    menu.draw();

    oled_.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled_.getLine(1, buf, sz); assertStringCaseEqual("", buf);
    oled_.getLine(2, buf, sz); assertStringCaseEqual("", buf);
    oled_.getLine(3, buf, sz); assertStringCaseEqual("", buf);
}

testF(AccountsMenuTest, draw_single_account)
{
    Account acc {"acc0", "login", "passwd"};
    assertTrue(ms_.add(acc));

    AccountsMenu menu(oled_, userInputs_, ms_);

    menu.draw();

    oled_.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled_.getLine(1, buf, sz); assertStringCaseEqual("acc0", buf);
    oled_.getLine(2, buf, sz); assertStringCaseEqual("", buf);
    oled_.getLine(3, buf, sz); assertStringCaseEqual("", buf);
}

testF(AccountsMenuTest, draw_more_accounts_then_visible)
{
    std::array<Account, 4> aa {{
        {"acc0", "login0", "passwd0"},
        {"acc1", "login1", "passwd1"},
        {"acc2", "login2", "passwd2"},
        {"acc3", "login3", "passwd3"}
    }};

    for (auto acc : aa) {
        assertTrue(ms_.add(acc));
    }

    AccountsMenu menu(oled_, userInputs_, ms_);
    menu.activate();

    assertTrue(aa[0] == menu.selected());
    menu.draw();

    oled_.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled_.getLine(1, buf, sz); assertStringCaseEqual("acc0", buf);
    oled_.getLine(2, buf, sz); assertStringCaseEqual("acc1", buf);
    oled_.getLine(3, buf, sz); assertStringCaseEqual("acc2", buf);

    userInputs_.tilt(DeviceInputs::UserAction::down); // acc0 ->acc1
    assertTrue(aa[1] == menu.selected());

    userInputs_.tilt(DeviceInputs::UserAction::down); // acc1 ->acc2
    assertTrue(aa[2] == menu.selected());

    userInputs_.tilt(DeviceInputs::UserAction::down); // acc2 ->acc3
    assertTrue(aa[3] == menu.selected());

    menu.draw();

    memset(buf, 0, sz);
    oled_.getLine(0, buf, sz); assertStringCaseEqual("Accounts", buf);
    oled_.getLine(1, buf, sz); assertStringCaseEqual("acc1", buf);
    oled_.getLine(2, buf, sz); assertStringCaseEqual("acc2", buf);
    oled_.getLine(3, buf, sz); assertStringCaseEqual("acc3", buf);
}

#endif // defined(EPOXY_DUINO)
