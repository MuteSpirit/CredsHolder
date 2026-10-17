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
#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/array.h>

// Avoid double definition:
//   fatfs.h includes ff.h which includes ff-inc.h which again include ff.h
//   FATFS_HEADER_ONLY_IMPL definition allows avoid include ff-inc.h by ff.h
#define FATFS_HEADER_ONLY_IMPL
#if defined(USE_TINYUSB)
#  undef USE_TINYUSB
#  include <fatfs.h>
#  define USE_TINYUSB
#else
#  include <fatfs.h>
#endif

// #include "src/auth.hpp"
// #include "src/cli.hpp"
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
#include "src/snake_game.hpp"

#include "src/sqlite.hpp"
#include "src/model/sql/account_storage.hpp"
#include "src/sqlite3_vfs_over_fatfs.hpp"

#if defined(ARDUINO_ARCH_NRF52)

#include "src/nrf52840/keyboard.hpp"
TinyUsbKeyboard keyboard;

#else

#include "src/arduino/keyboard.hpp"
ArduinoKeyboard keyboard;

#endif

////////////////////////////////////////////////////////////////////////////////
// Workaround for Seeduino linker error about undefined __throw_length_error
// namespace std {
//   void __throw_length_error(char const*) {
//   }
// }

////////////////////////////////////////////////////////////////////////////////
// Constants

// Baud rate for serial port
#define SERIAL_BAUD_RATE 115200

#define SHOW_SPLASHSCREEN 1000 // ms

////////////////////////////////////////////////////////////////////////////////
// Globals
using namespace creds_holder;

/// For TinyFATFS: Logical partition to physical device mapping table
PARTITION VolToPart[] {
    {0, 0}, // 0: -> 1st physical disk, 1st FAT partition
    {0, 1}  // 1: -> 1st physical disk, 2nd FAT partition
};

Settings settings;

SSD1306I2C oled;

CredsHolderOutputs deviceOutputs;

CredsHolderInputs userInputs(deviceOutputs);

// PasswordWandAuth authenticator;

// TODO: move to callback
// assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

// MemoryBlockStorage<4098, 32> mbs;
// ModelStorage<Account> modelStore(mbs);

fatfs::RamIO<256 /*sectors*/> drv{512 /*bytes per sector*/};
uint8_t work[FF_MAX_SS] = {0};
fatfs::FatFs fs(drv);

AccountSqlStorage modelStore;

// Create on demand ???
// AuthForm authForm(oled, userInputs, authenticator);
AccountsMenu<AccountSqlStorage> accountsMenu(oled, userInputs, modelStore);
AccountMenu accMenu(oled, userInputs, keyboard);
SettingsMenu settingsMenu(oled, userInputs, settings);

SnakeGame tutorial(oled, userInputs);

DisplayUI ui(oled, userInputs/*, authForm*/, tutorial, accountsMenu, accMenu, settingsMenu);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
setup()
{
    if (deviceOutputs.setup()) {
        if (Serial) { Serial.println(F("DeviceOutputs::setup failed")); }
    }

    // Init keyboard before Serial to avoid troubles
    keyboard.setup();

    Serial.begin(SERIAL_BAUD_RATE);

    // Waiting Serial initialization forever means stuck forever if Arduino IDE
    // Serial Monitor (or analog app) does not run
    // while (!Serial) { delay(50); };
    delay(50);

    // <debug>
    etl::array<Account, 3> aa {{
        {1, "name0", "url0", "user0", "passwd0"},
        {2, "name1", "url1", "user1", "passwd1"},
        {3, "NAME2", "url2", "USER2", "PASSWD2"}
    }};
    //
    // Create MBR
    memset(work, 0, sizeof(work));

    LBA_t plist[] = {100, 0};
    fs.f_fdisk(0, plist, work);
 
    // Create FS
    fs.f_mkfs("0:", nullptr, work, sizeof(work));

    fatfs::vfs::register_mount(nullptr, &fs, "/rom0");

    do {
        if (SQLITE_OK != sqlite3_initialize()) {
            if (Serial) { Serial.println(F("sqlite3_initialize fail")); }
            break;
        }

        sqlite3 *db = nullptr;
        if (SQLITE_OK != sqlite3_open_v2("/rom0/creds.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, fatfs::vfs::name())) {
            if (Serial) { Serial.println(F("sqlite3_open_v2 fail")); }
            break;
        }
        if (nullptr == db) {
            if (Serial) { Serial.println(F("db is NULL")); }
            break;
        }

        modelStore.init(db);

        if (!modelStore.initSchema()) {
            if (Serial) { Serial.println(F("initSchema fail")); }
            break;
        }

        for (auto& acc : aa) {
            modelStore.add(acc);
        }
    } while(0);
    // </debug>

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
        if (Serial) { Serial.println(F("MPU6050 Error")); }
    }

    // cli_init(settings.cli_turn_on_);

    ui.setup();
}

// TODO: Need design for "power cut" issue
    // sqlite3_close_v2(db);
    // sqlite3_shutdown();
    // fatfs::vfs::unregister_mount(nullptr, "/rom0");
    // fatfs::FR_OK == fs.f_unmount("");

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
    ui.loop_step();
}
