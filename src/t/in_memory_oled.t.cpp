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
#if defined(EPOXY_DUINO)
#include "test/in_memory_oled.hpp"
#include <AUnitVerbose.h>

test(in_mem_oled_ctor)
{
    // Must cause compilation error:
    //   OledInMem<0, 0> oled;
    //   OledInMem<1, 0> oled;

    OledInMem<1, 2> screen;
    assertEqual(1, screen.getCols());
    assertEqual(2, screen.getRows());
    assertEqual((size_t)3, screen.getBufSize());
};

test(in_mem_oled_cast_to_base_class_oled)
{
    OledInMem<1, 1> screen;
    { Oled *oled = &screen; (void)oled; }
    { Oled& oled = screen;  (void)oled; }
};

test(in_mem_oled_print_one_letter)
{
    OledInMem<1, 1> o;
    o.print("H");
    assertStringCaseEqual("H", o.getBuffer());
};

test(in_mem_oled_print_two_letters)
{
    OledInMem<2, 1> o;
    o.print("He");
    assertStringCaseEqual("He", o.getBuffer());
};

test(in_mem_oled_println_empty_string)
{
    OledInMem<1, 1> o;
    o.println("");
    assertStringCaseEqual("", o.getBuffer());
};

test(in_mem_oled_println_non_empty_string)
{
    OledInMem</*cols*/ 3, 2> o;
    o.println("abc");
    o.println("def");
    assertStringCaseEqual("abcdef", o.getBuffer());
};

test(in_mem_oled_print_cut_str)
{
    OledInMem<2, 1> o;
    o.print("Hello");
    assertStringCaseEqual("He", o.getBuffer());
};

test(in_mem_oled_print_cut_str_and_println_jump_next_row)
{
    OledInMem<2, 2> o;
    o.print("Hello");
    o.println("");
    o.print("World");
    assertStringCaseEqual("HeWo", o.getBuffer());
};

#endif // defined(EPOXY_DUINO)
