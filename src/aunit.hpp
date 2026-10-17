#pragma once
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

/// @file aunit.hpp
/// AUnitVerbose.h wrapper header. It contains additional assert macros
/// working with types not expected by original AUnit, e.g. etl::string

#include <AUnitVerbose.h>

namespace aunit {

#define assertEtlStringCaseEqual(expected, actual) \
    assertStringCaseEqual((expected).c_str(), (actual).c_str())

} // namespace aunit
