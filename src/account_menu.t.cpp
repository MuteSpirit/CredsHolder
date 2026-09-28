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
#include "account_menu.cpp"

#include "device_inputs.hpp"
#include "t/in_memory_oled.hpp"
#include "t/imitated_user_unputs.hpp"
#include "t/keyboard.cpp"

// Must be included as the last one to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

test(account_menu_ctor_and_init)
{
    OledInMem<1, 1> oled;
    ImitatedUserInputs userInputs;
    ImitatedKeyboard keyboard;

    AccountMenu accMenu(oled, userInputs, keyboard);

    Account acc{"name", "login", "passw"};
    accMenu.account(acc);
};

test(account_menu_draw_with_empty_acc)
{
    OledInMem<1, 1> oled;
    ImitatedUserInputs userInputs;
    ImitatedKeyboard keyboard;

    AccountMenu accMenu(oled, userInputs, keyboard);
};

test(account_menu_draw_with_acc)
{
    OledInMem<11, 4> oled;
    ImitatedUserInputs userInputs;
    ImitatedKeyboard keyboard;

    AccountMenu accMenu(oled, userInputs, keyboard);

    Account acc{"accName", "accLogin", "accPassw"};
    accMenu.account(acc);

    accMenu.draw();

    constexpr uint8_t sz = 16;
    char buf[sz] = {0};

    oled.getLine(0, buf, sz); assertStringCaseEqual("N: accName" , buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("L: accLogin", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("P: accPassw", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual(""           , buf);
};

test(account_menu_type_login)
{
    // Given
    OledInMem<11, 4> oled;
    ImitatedUserInputs userInputs;
    ImitatedKeyboard keyboard;

    AccountMenu accMenu(oled, userInputs, keyboard);

    Account acc{"accName", "accLogin", "accPassw"};
    accMenu.account(acc);
    //
    // When
    userInputs.tilt(DeviceInputs::UserAction::right);

    // without activate() nothing should happen
    assertStringCaseEqual("", keyboard.istream());
    //
    // but after register callbacks...
    accMenu.activate();

    // menu should work
    userInputs.tilt(DeviceInputs::UserAction::right);
    assertStringCaseEqual("accLogin", keyboard.istream());
};


test(account_menu_type_password)
{
    // Given
    OledInMem<11, 4> oled;
    ImitatedUserInputs userInputs;
    ImitatedKeyboard keyboard;

    AccountMenu accMenu(oled, userInputs, keyboard);
    accMenu.activate();

    Account acc{"accName", "accLogin", "accPassw"};
    accMenu.account(acc);
    //
    // When
    userInputs.tilt(DeviceInputs::UserAction::down);
    userInputs.tilt(DeviceInputs::UserAction::right);

    assertStringCaseEqual("accPassw", keyboard.istream());
};

#endif // defined(EPOXY_DUINO)
