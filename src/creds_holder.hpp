// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

// Moved into Makefile to define it globally
// #if !defined ETL_NO_STL
// #define ETL_NO_STL
// #endif

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
