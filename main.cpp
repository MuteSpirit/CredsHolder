// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#if defined(ARDUINO_ARCH_NRF52)
#define NRF52840
#endif

#include "src/auth.hpp"
#include "src/cli.hpp"
#include "src/model.hpp"
#include "src/model_storage.hpp"
#include "src/ssd1306_oled.hpp"
#include "src/version.hpp"
#include "src/display_ui.hpp"
#include "src/device.hpp"
#include "src/accounts_menu.hpp"
#include "src/settings_menu.hpp"
#include "src/settings.hpp"
#include "src/auth_form.hpp"
#include "src/model_storage.hpp"
#include "src/memory_block_storage.hpp"

#if defined(ARDUINO_ARCH_NRF52)

#include "src/nrf52840/keyboard.hpp"
TinyUsbKeyboard keyboard;

#else

#include "src/arduino/keyboard.hpp"
ArduinoKeyboard keyboard;

#endif

// Baud rate for serial port
#define SERIAL_BAUD_RATE 38400

#define SHOW_SPLASHSCREEN 1000 // ms

Settings settings;

SSD1306I2C oled;

CredsHolderInputs userInputs;

PasswordWandAuth authenticator;

// TODO: move to callback
// assertTrue(ebs.init(encStoreKey, encStoreKeyLen));

MemoryBlockStorage<128, 16> mbs;
ModelStorage<Account> modelStore(mbs);

// Create on demand ???
AuthForm authForm(oled, userInputs, authenticator);
AccountsMenu accMenu(keyboard, oled, userInputs, settings, modelStore);
SettingsMenu settingsMenu(oled, userInputs, settings);

DisplayUI ui(oled, userInputs, authForm, accMenu, settingsMenu);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
setup()
{
  pinMode(4, INPUT_PULLUP); // DEBUG

  userInputs.setup();

  oled.setup();
  oled.setFont(u8x8_font_chroma48medium8_r);

  // oled.setFont(System5x7);                                                // perfect, slightly smaller than Arial14

  oled.clear();

  // enable Serial after init CLI to avoid racing between CLI init and CLI usage
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  print_welcome(oled);
  delay(SHOW_SPLASHSCREEN);

  print_welcome(Serial);

  ui.ui_setup();

  cli_init(settings.cli_turn_on_);
}

void
loop()
{
  static bool startup_delay_done = false;
  if (!startup_delay_done) {
    delay(750);
    startup_delay_done = true;
  }

  cli_loop_step();
  userInputs.loop_step();
}
