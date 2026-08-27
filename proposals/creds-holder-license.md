<!--
Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.

Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the section entitled "GNU
Free Documentation License".
-->

# CredsHolder code license

## Introduction

This document is related to project license aspect. 

There are several contradiction requirements for that in [CredsHolder HLD Doc](creds-holder.md#Requirements) and it's needed to find optimal solution for that.

## Abbreviations and terms

| Term | Description |
| :---: | :---: |
| POC | Proof Of Concept. Prototype application to proof proposals idea(s) |
| TRNG | A True Random Number Generator |
| MBR | Master Boot Record |

## Requirements

| No | Re User Story | Requirement |
| --- | --- | --- |
| R-21 | S-8, S-12, S-14, S-17 | All source code and schematics MUST be OpenSource to allow any User assemble device and modify/build firmware. |
| R-30 | S-15 | CredsHolder code/FW MUST allow to extend encryption/hash algorithm and add something else. |
| R-34 | S-16 | CredsHolder License MUST allow commercial usage to allow any company make and sale CredsHolder based product. |
| R-38 | S-17 | All Vendors MUST public own changes into CredsHolder sources...<br/>or data on extractable storage MUST be reusable by another CredsHolder device. |

## Background

### Licenses

### Useful Libraries

According to several proposal documents and POCs made previously next libraries should be useful for project:

| Theme | Library | URL | Comment | License |
| --- | --- | --- | --- | --- |
| USB HID keyboard<b/>Bluetooth keyboard<b/>USB MassStorage | Adafruit_TinyUSB_Arduino | https://github.com/Adafruit/adafruit_tinyusb_arduino | Supports NRF52840 | MIT |
| I2C | ArduinoCore-avr/Wire | https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/Wire | Needed for libraries works with OLED and MPU6050 | GPL-v2 or GPL-v2.1 or later |
| SSD1306 OLED display | SSD1306Ascii | https://github.com/greiman/ssd1306ascii | | MIT | 
| SSD1306 OLED display | Adafruit_SSD1306 | https://github.com/adafruit/Adafruit_SSD1306 | | BSD |
| NRF52840 internal NVM storage | arduino-NVM | https://github.com/d00616/arduino-NVM | | LGPL 2.1 only |
| Raw access microSD CardReader SPI | ArduinoCore-avr/SPI | https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/SPI | | GPL-v2 or LGPL-v2.1 only |
| Raw access microSD CardReader SPI | Adafruit_SPIFlash | https://github.com/adafruit/Adafruit_SPIFlash | | MIT |
| FAT16, FAT32, MBR | SdFat | https://github.com/greiman/SdFat | | MIT |
| Position sensor (accelerator + gyroscope) MPU6050 | I2CDev | https://github.com/jrowberg/i2cdevlib | | MIT | 
| tilt detection | ? | ? | Not yet founded | ? |
| CLI over UART | SimpleCLI | https://github.com/spacehuhntech/SimpleCLI | | MIT |
| Cryptography acceleration<br/>TRNG | Adafruit_nRFCrypto | https://github.com/adafruit/Adafruit_nRFCrypto | Supports NRF52840 | MIT |
| VeraCrypt volume | VeraCrypt | https://github.com/veracrypt/VeraCrypt | C API | Apache License 2.0 and the TrueCrypt License version 3.0 |
| VeraCrypt volume | VeraCrypt-SDK | https://github.com/veracrypt/VeraCrypt | C++ API | Apache License 2.0 |
| LUKS volume header | libluksde | https://github.com/libyal/libluksde | GPL-v3.0, LGPL-v3.0 | 
| LUKS volume header | libcryptsetup | https://gitlab.com/cryptsetup/cryptsetup | GPL-v2.0-or-later | 
| SQLite3 | SQLite | https://sqlite.org/ | | Public Domain (copyleft if reuse original code) |
| Credentials Storage Alternative | Password Gorilla | https://github.com/zdia/gorilla | | GPLv2 or later |
| Credentials Storage Alternative | KeePassXC | https://github.com/keepassxreboot/keepassxc | | GPLv2 or GPLv3 |
| Credentials Storage Alternative | gopass | https://github.com/gopasspw/gopass | | MIT |
| Credentials Storage Alternative | PasswordSafe | https://github.com/pwsafe/pwsafe | [compatible with GPL](https://www.gnu.org/licenses/license-list.html#ArtisticLicense2) | Artistic License 2.0 |
| UI interface | Adafruit GFX | https://github.com/adafruit/adafruit-gfx-library | | BSD | 
| secure boot, non immutable bootloader | Nordic nRF Connect SDK | https://github.com/nrfconnect/sdk-nrf/ | Can be used with mentioning copyright notice.<br/>bootloader is separate code and binary and has no license relation to application code license | LicenseRef-Nordic-5-Clause |
| NRF52840 board support in Arduino IDE | | https://github.com/pdcook/nRFMicro-Arduino-Core | | GPL-v2.1 or later |
| Unit Tests on target board | AUnit | https://github.com/bxparks/AUnit | | MIT |
| Unit Tests on host | EpoxyDuino | https://github.com/bxparks/EpoxyDuino | Some mock libraries for Arduino are supported but no mentions about nRF52. Need to be adapted. | MIT |
| (alternative to position sensor UI) Rotary Encoder | RotaryEncoder | https://github.com/mathertel/RotaryEncoder | will be needed if UI based on MPU6050 failed | BSD |
| Tap/knock detection via Piezo Sensor | | https://docs.arduino.cc/built-in-examples/sensors/Knock/ | Separate lib is not needed | Public Domain |
| Morse code via vibrations | | | Separate lib is not needed | |

### Code Licenses

Next licenses is compatible with project using MIT/BSD license:
* MIT
* BSD
* Apache License 2.0

LGPL is also usable if linked dynamically with our application. But not for Arduino like embedded systems - all libraries are linked statically if e.g. you develop code in Arduino IDE.

License as GPL v2/v3 requires public sources of application using library under GPL license. So GPL v2.x/v3.x are not usable in project with MIT license.

See [GPL licenses compatibility](https://www.gnu.org/licenses/gpl-faq.html#AllCompatibility).

See [GNU article about licenses](https://www.gnu.org/licenses/license-list.html).

## Contradictions

* Arduino/nRF52 based embedded projects cannot use dynamically linked libraries.
* Not all GPL licenses are compatible, e.g. (L)GPLv3-or-later cannot use GPLv2-only and vice versa - see [compatibility table](https://www.gnu.org/licenses/gpl-faq.html#AllCompatibility)
* Some core libraries like [NRF52840 board support in Arduino IDE](https://github.com/pdcook/nRFMicro-Arduino-Core) use GPL license and there is no alternative for that.
* Project with MIT license cannot use libraries under GPL licenses.

## Proposals

Change license to GPL v3 or later versions.

Reasons:
* Compatibility with any license used in libraries needed in project
* Force proprietary project based on CredsHolder (I hope that they will appear eventually :-) ) share their code changes with OpenSource community.

Advantages:
* Ability to use libraries with any Open Source licenses. That will decrease project development effort.
* Requirement R-30 and R-34 will be able to met too if company which make a CredsHolder based product will open own source changes.
  * According to [GPL FAQ](https://www.gnu.org/licenses/gpl-faq.html#DoesTheGPLAllowMoney) the code can be sold. [Trezor Wallet](https://trezor.io/) and [Mooltipath](https://www.themooltipass.com/) are examples of such commercial schema.
* Security and encryption is very complex themes and keep them as Open Source allows to review the code and start to trust it.
* Increasing chances to meet requirement R-38: if commercial project code changes will be published then it'll be theoretically possible to write recover/import code for encrypted storage of proprietary device(s).
