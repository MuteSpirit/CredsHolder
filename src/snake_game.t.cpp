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
#include "snake_game.hpp"

// Must be included as the last one to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

#include "t/in_memory_oled.hpp"
#include "t/imitated_user_unputs.hpp"

test(snake_game_ctor)
{
    OledInMem<1, 1> oled;
    ImitatedUserInputs userInputs;

    SnakeGame sg(oled, userInputs);
}
