# TODO

| :blue_book: | Reason(s) |
| --- | :--- |
| 1 | Tracking tasks is in Markdown to avoid dependency on GitHub embedded bugtracker |
| 2 | Task list is present in each repository backup |

## Abbrevs, Terms, etc.

"Device" - CredsHolder common title in proposals, manuals and other resources

## Sub-tasks

* [x] CONTRIBUTING.md: with development environment settings, docs style, folder structure
* [x] docs(CONTRIBUTING.md): description of HLD principals
* [x] docs: collect project requirements
* [x] docs(CONTRIBUTING.md): description of LLD principals
* [x] Propose Device UI
  * [x] POC1. Tilt detection on Arduino
  * [x] POC2. Tilt detection on NRF52840
* [x] Propose Device HW storage
* [x] Propose Device (encrypted) data storage
* [x] Propose Device file system
* [x] Propose menu navigation
  * without auth
  * menu Accounts
  * typing login/password
  * no encryption, no storage on microSD, hard-coded accounts
* [x] vibration feedback on tilt up to enough angle
* [x] Implement tutorial
* [x] Implement new way for accept/enter action - via tap/knock
* [~] Refactoring
  * [ ] Make code less Arduino: no F("<string>") or PROGMEM usage or configurable
  * [ ] Make autoformatting (config + nvim lsp config + neovim-dev-env image)
  * [ ] Add -MD compiler option and add more correct dependencies in Makefile
  * [x] Put '#pragma once' on first line before any license banners
  * [ ] build test objects/exec in separate folder
  * [x] build .o files for tests using all .cpp files, not .t.cpp only
  * [ ] scope code within new C++ namespace "creds_holder"
  * [x] auto generation compiler_commands.json
  * [ ] Implement "insulation" as template class
  * [ ] Include "creds_holder.hpp" globally via CFLAGS/CPPFLAGS/CXXFLAGS instead of explicit one in each source file
  * [ ] Build Adafruit_nRFCrypto under AUnit
  * [ ] Check U8x8 log class - maybe it's better in memory debug oled then OledInMem
  * [ ] Exclude .t.cpp files from build for target platform and avoid "if defined(EPOXY_DUINO)" guards usage
  * [ ] Add memory sanitizer compiler options to catch memory access troubles. Think about Valgrind
* BSP
  * [ ] Why Seeduino try to use std_vector.h which use exception and require __throw_length_error definition ??!
  * [ ] Replace -Ofast to maybe -Os or find way to strip binaries
  * [ ] Just to the latest available C/C++ standard for compilation
  * [ ] Add SQLite3 library
  * [ ] Add TinyFATFS
  * [ ] Add SQLite3 VFS library
* [~] Implement read file from microSD card with partition "storage" formatted with FAT32
  * [ ] Check is it needed to implement own fatfs::SPIClass using EEPROM_SPI_EE
  * [ ] !!! I had to change TinyFATFS sources to enable FF_MULTI_PARTITION. Do something to use it by default
  * [ ] Check what does mean fatfs::MKFS_PARM::n_fat and is it needed to use 2 or not
  * [x] Build SQLite3 amalgamation source from last official sources to be able predefine all needed macro and remove unneeded parts. Use https://github.com/siara-cc/esp32_arduino_sqlite3_lib/blob/master/src/config_ext.h as example
* [ ] Implement formatting partition "storage" as VeraCrypt volume
* [ ] Implement reading from VeraCrypt volume
  * [ ] migrate from Crypto to nRFCrypto (add abstraction?)
* [ ] Implement PIN on LUKS header in internal storage
* [ ] Implement set PIN by User
* [ ] propose authentication
  * (blocked) vibration motor module is in delivery
* [ ] Implement calibration
* [ ] Implement authentication by hard-coded pin
* [ ] propose object storage
  * [~] find OpenSource applications which code may be reused
* [~] propose secure burn/boot
* [-] propose interface between Device and user device (PC/smartphone)
* [ ] propose device case
* [ ] propose OS (sketch only or RTOS or ...)
* [ ] propose plugins schema in SW
* [ ] propose schematic
* [ ] implement tilt detection in non vertical position, e.g. lying on the sofa
* [ ] implement tilt detection during moving in vehicle 
* [ ] Show password as QR code (when device will be on battery)
* [ ] Mark A0-A2 pins in Fritzing part for nRF52840 Pro Micro
* [ ] ROADMAP.md: propose MVP and Roadmap
* [ ] CONTRIBUTING.md: development process principals
* [~] TODO.md: plan development
* [ ] TODO.md: estimate development
* [ ] Make forks of all dependency libraries and fix all compilation warnings
  * It's reasonable to take "latest and greatest" version of libraries related to security in spite of versions available in Arduino IDE library index. Maybe other libraries - too. Just to have fixed and tested versions of dependencies.
  * To improve cloning speed it's possible to use [shallow sub-modules](https://git-scm.com/docs/git-clone#Documentation/git-clone.txt---shallow-submodules)
  * include <fatfs.h> give redefinition if made from C and from C++ files because (?) only pragma once without include guards usage
  * There is separate page on chan-fatfs website with latest patches. Need to take them too.
  * find a way to avoid TinyFATFS usage and smart way to convert original FatFs into C++ classes FatFs and IO
* [ ] Fix multiple interrupts support on nRF52840 in BSP 
