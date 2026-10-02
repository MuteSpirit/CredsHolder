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
#pragma once

// Keep Embedded Template Library defines in Makefile only does not help to build
// sketch using Arduino IDE. So let keep them here also.
#if !defined ETL_NO_STL
#define ETL_NO_STL
#endif

#if !defined DETL_NO_INITIALIZER_LIST
#define DETL_NO_INITIALIZER_LIST
#endif

#if defined(ARDUINO_ARCH_NRF52)
#  if !defined(NRF52)
#    define NRF52
#  endif

#  if !defined(NRF52840_XXAA)
#    define NRF52840_XXAA
#  endif

#  if !defined(NRF52840)
#    define NRF52840
#  endif

#  if !defined(CFG_TUD_ENABLED)
#    define CFG_TUD_ENABLED 1
#  endif

#  if !defined(CFG_TUD_HID)
#    define CFG_TUD_HID 1
#  endif

#  if !defined(USE_TINYUSB)
#    define USE_TINYUSB 1
#  endif
#endif
