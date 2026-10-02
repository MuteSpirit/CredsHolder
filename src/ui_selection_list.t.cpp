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
#include "Embedded_Template_Library.h"
#include "etl/array.h"
#include "etl/vector.h"
#include "ui_selection_list.hpp"
#include "t/in_memory_oled.hpp"

// Must be included as the last header to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

////////////////////////////////////////////////////////////////////////////////
class UISelectionListTestHelper
{
public:
    template<typename Iterator, typename T>
    static uint8_t visible(UISelectionList<Iterator, T>& sl) { return sl.visible_; }
};

////////////////////////////////////////////////////////////////////////////////
test(ui_sl_ctor_iterator)
{
    OledInMem<16, 4> oled;
    etl::array<const char*, 3> sa;

    UISelectionList<typename etl::array<const char*, 3>::iterator, const char*> sl(oled, "title", sa.begin(), sa.end());
}

test(ui_sl_ctor_const_iterator)
{
    OledInMem<16, 4> oled;
    etl::array<const char*, 3> sa;

    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, "title", sa.cbegin(), sa.cend());
}

test(ui_sl_visible)
{
    OledInMem<16, 4> oled;
    etl::array<const char* const, 3> sa {"a", "b", "c"};
    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, "title", sa.cbegin(), sa.cend());

    assertEqual(4, UISelectionListTestHelper::visible(sl));
}

test(ui_sl_next)
{
    OledInMem<16, 4> oled;
    using CStr = char[2];
    using CStrPtrs = etl::array<CStr, 3>;
    CStrPtrs sa {"a", "b", "c"};

    UISelectionList<typename CStrPtrs::iterator, CStr> sl(oled, "title", sa.begin(), sa.end());
    assertStringCaseEqual("a", sl.selected());

    sl.next();
    assertStringCaseEqual("b", sl.selected());

    sl.next();
    assertStringCaseEqual("c", sl.selected());
}

test(ui_sl_with_one_item_and_next_jump_to_start)
{
    OledInMem<16, 4> oled;
    etl::array<const char*, 1> sa {"a"};

    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, "title", sa.cbegin(), sa.cend());

    assertStringCaseEqual("a", sl.selected());

    sl.next();
    assertStringCaseEqual("a", sl.selected());

    sl.next();
    assertStringCaseEqual("a", sl.selected());
}

test(ui_sl_with_two_items_and_next_jump_to_start)
{
    OledInMem<16, 4> oled;
    etl::array<const char*, 2> sa {"a", "b"};

    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, "title", sa.cbegin(), sa.cend());

    assertStringCaseEqual("a", sl.selected());

    sl.next();
    assertStringCaseEqual("b", sl.selected());

    sl.next();
    assertStringCaseEqual("a", sl.selected());
}

test(ui_sl_with_one_item_and_prev_jump_to_start)
{
    OledInMem<16, 4> oled;
    etl::array<const char*, 1> sa {"a"};

    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, "title", sa.cbegin(), sa.cend());

    assertStringCaseEqual("a", sl.selected());

    sl.prev();
    assertStringCaseEqual("a", sl.selected());

    sl.prev();
    assertStringCaseEqual("a", sl.selected());
}

test(ui_sl_with_two_items_and_prev_jump_to_end)
{
    // Given
    OledInMem<16, 4> oled;
    etl::array<const char*, 2> sa {"a", "b"};
    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, "title", sa.cbegin(), sa.cend());

    assertStringCaseEqual("a", sl.selected());
    // When
    sl.prev();
    // Then
    assertStringCaseEqual("b", sl.selected());

    sl.prev();
    assertStringCaseEqual("a", sl.selected());
};

test(ui_sl_draw_without_title)
{
    // Given
    OledInMem<1, 4> oled;
    etl::array<const char*, 3> sa {"a", "b", "c"};
    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, nullptr, sa.cbegin(), sa.cend());
    // When
    sl.draw();
    // Then
    assertStringCaseEqual("abc", oled.getBuffer());
};

test(ui_sl_draw_with_title)
{
    // Given
    OledInMem<1, 4> oled;
    etl::array<const char*, 3> sa {"a", "b", "c"};
    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, "t", sa.cbegin(), sa.cend());
    // When
    sl.draw();
    // Then
    assertStringCaseEqual("tabc", oled.getBuffer());
};

test(ui_sl_next_with_null_start_and_end)
{
    // Given
    OledInMem<1, 4> oled;
    UISelectionList<typename etl::array<const char* const, 3>::iterator, const char*> sl(oled, nullptr, nullptr, nullptr);
    // When
    assertNoFatalFailure(sl.next());
};

test(ui_sl_draw_empty_container)
{
    // Given
    OledInMem<1, 4> oled;
    etl::vector<const char*, 1> sa;

    UISelectionList<typename decltype(sa)::const_iterator, const char*> sl(oled, "A", sa.cbegin(), sa.cend());

    // When
    sl.draw();

    // Then
    constexpr uint8_t sz = 16;
    char buf[sz] = {0};

    oled.getLine(0, buf, sz); assertStringCaseEqual("A", buf);
    oled.getLine(1, buf, sz); assertStringCaseEqual("", buf);
    oled.getLine(2, buf, sz); assertStringCaseEqual("", buf);
    oled.getLine(3, buf, sz); assertStringCaseEqual("", buf);
};

test(ui_sl_draw_with_title_and_longer_visible_and_next_up_to_last_item)
{
    // Given
    OledInMem<1, 2> oled;
    etl::array<const char*, 3> sa {"a", "b"};
    UISelectionList<typename decltype(sa)::const_iterator, const char*> sl(oled, "t", sa.cbegin(), sa.cend());
    // When
    sl.next(); // a -> b
    sl.draw();
    // Then
    assertStringCaseEqual("tb", oled.getBuffer());
};

#endif // defined(EPOXY_DUINO)
