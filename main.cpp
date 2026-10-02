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
#include "src/creds_holder.hpp"

#include "Embedded_Template_Library.h"
#include "etl/array.h"

// #include "src/auth.hpp"
#include "src/cli.hpp"
#include "src/model/account.hpp"
#include "src/model/iterator.hpp"
#include "src/model/storage.hpp"
#include "src/ssd1306_oled.hpp"
#include "src/version.hpp"
#include "src/display_ui.hpp"
#include "src/device.hpp"
#include "src/accounts_menu.hpp"
#include "src/account_menu.hpp"
#include "src/settings_menu.hpp"
#include "src/settings.hpp"
// #include "src/auth_form.hpp"
#include "src/memory_block_storage.hpp"

#if defined(ARDUINO_ARCH_NRF52)

#include "src/nrf52840/keyboard.hpp"
TinyUsbKeyboard keyboard;

#else

#include "src/arduino/keyboard.hpp"
ArduinoKeyboard keyboard;

#endif

// Baud rate for serial port
#define SERIAL_BAUD_RATE 115200

#define SHOW_SPLASHSCREEN 1000 // ms

Settings settings;

SSD1306I2C oled;

CredsHolderInputs userInputs;

// PasswordWandAuth authenticator;

// TODO: move to callback
// assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

MemoryBlockStorage<4098, 32> mbs;
ModelStorage<Account> modelStore(mbs);

// Create on demand ???
// AuthForm authForm(oled, userInputs, authenticator);
AccountsMenu accountsMenu(oled, userInputs, modelStore);
AccountMenu accMenu(oled, userInputs, keyboard);
SettingsMenu settingsMenu(oled, userInputs, settings);

DisplayUI ui(oled, userInputs/*, authForm*/, accountsMenu, accMenu, settingsMenu);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
setup()
{
    // <debug>
    etl::array<Account, 3> aa {{
        {"name0", "user0", "passwd0"},
        {"name1", "user1", "passwd1"},
        {"NAME2", "USER2", "PASSWD2"}
    }};
    for (auto& acc : aa) {
        modelStore.add(acc);
    }
    // </debug>

    // Init keyboard before Serial to avoid troubles
    keyboard.setup();

    Serial.begin(SERIAL_BAUD_RATE);

    // Waiting Serial initialization forever means stuck forever if Arduino IDE
    // Serial Monitor (or analog app) does not run
    // while (!Serial) { delay(50); };
    delay(50);

    // debug: hard-coded Accounts has been added
    // TODO: use pattern Observer
    accountsMenu.notifyModelStoreUpdated();

    if (Serial) {
        print_welcome(Serial);
    }

    oled.setup();
    oled.setFont(u8x8_font_8x13_1x2_f);

    oled.clear();
    oled.home();
    print_welcome(oled);
    oled.display();
    delay(SHOW_SPLASHSCREEN);

    // if (Serial) {
    //     for (ModelIterator<Account> it = modelStore.cbegin(); it != modelStore.cend(); ++it) {
    //         Serial.println((*it).name);
    //     }
    // }

    if (!userInputs.setup()) {
        if (Serial) { Serial.println(F("MPU6050 Error"));}
    }
    // cli_init(settings.cli_turn_on_);

    ui.setup();
}

void
loop()
{
    static bool startup_delay_done = false;
    if (!startup_delay_done) {
        delay(750);
        startup_delay_done = true;
        //
        // oled.clear();
        // oled.home();
        // oled.println("DEBUG");

    }
    // TODO: auto register "loop_step" handlers ???

    // <debug>
    // if (Serial) Serial.println(F("."));
    // delay(1000);

    // cli_loop_step();

    userInputs.loop_step();
}
