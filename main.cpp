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

// #include "src/auth.hpp"
#include "src/cli.hpp"
#include "src/account.hpp"
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
AccountsMenu accountsMenu(oled, userInputs, settings, modelStore);
// AccountMenu accMenu(keyboard, oled, userInputs);
SettingsMenu settingsMenu(oled, userInputs, settings);

DisplayUI ui(oled, userInputs/*, authForm*/, accountsMenu/*, accMenu*/, settingsMenu);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
setup()
{
    // <debug>
    Account acc0 {"n0", "u0", "p0"};
    Account acc1 {"n1", "u1", "p1"};
    Account acc2 {"n2", "u2", "p2"};
    modelStore.add(acc0);
    modelStore.add(acc1);
    modelStore.add(acc2);

    // </debug>
    //
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) { delay(50); };
    print_welcome(Serial);
    //
    oled.setup();
    // oled.setFont(u8x8_font_chroma48medium8_r);
    // oled.setFont(u8x8_font_courR18_2x3_f);
    oled.setFont(u8x8_font_8x13_1x2_f);

    // oled.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font

    oled.clear();
    oled.home();
    print_welcome(oled);
    oled.display();
    delay(SHOW_SPLASHSCREEN);

    // if (Serial) {
    //     Account acc;
    //
    //     modelStore.get(0, acc);
    //     Serial.println(acc.name);
    //
    //     modelStore.get(1, acc);
    //     Serial.println(acc.name);
    //
    //     modelStore.get(2, acc);
    //     Serial.println(acc.name);
    // }

    ui.setup();

    if (!userInputs.setup()) {
        if (Serial) { Serial.println(F("MPU6050 Error"));}
    }
    // cli_init(settings.cli_turn_on_);
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
    // cli_loop_step();
    userInputs.loop_step();
    // <debug>
    // if (Serial) Serial.println(F("."));
    // delay(1000);
}
