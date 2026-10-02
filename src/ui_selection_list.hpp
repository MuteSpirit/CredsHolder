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
#include "creds_holder.hpp"
#include <Arduino.h>

#include "Embedded_Template_Library.h"  // This is required for any more etl import when using Arduino IDE
#include "etl/iterator.h"
#include "etl/memory.h"
#include "etl/string.h"
#include <inttypes.h>

#include "oled.hpp"

class UISelectionListTestHelper;


////////////////////////////////////////////////////////////////////////////////
template<typename Iterator, typename T>
class UISelectionList
{
public:
    UISelectionList(Oled& oled, const char* title, Iterator start, Iterator end);

    const T& selected() const;

    void next();
    void prev();

    /// Model Storage has been updated externally and iterators in selection list
    /// should be updated too
    void update(Iterator start, Iterator end);

    void draw();

protected:
    /// @return visible rows taking into account title (non)existence
    uint8_t visible() const;

protected:
    etl::string<255> title_;
    Oled& oled_;
    uint8_t visible_{0};

    Iterator firstItemIt_;
    Iterator endItemIt_;

    Iterator firstIt_;
    Iterator curIt_;

protected:
    friend class UISelectionListTestHelper;
};

////////////////////////////////////////////////////////////////////////////////
template<typename Iterator, typename T>
UISelectionList<Iterator, T>::UISelectionList(Oled& oled, const char* title, Iterator start, Iterator end)
    : title_(title)
    , oled_(oled)
    , visible_(1)
    , firstItemIt_(start)
    , endItemIt_(end)
    , firstIt_(start)
    , curIt_(start)
{
}

template<typename Iterator, typename T>
const T&
UISelectionList<Iterator, T>::selected() const
{
    return *curIt_;
}

template<typename Iterator, typename T>
void
UISelectionList<Iterator, T>::next()
{
    ++curIt_;
    if (curIt_ == endItemIt_) {
        curIt_ = firstItemIt_;
        firstIt_ = firstItemIt_;
    } else {
        // TODO: check me
        if (etl::distance(firstIt_, curIt_) >= visible()) {
            ++firstIt_;
        }
    }
}

template<typename Iterator, typename T>
void
UISelectionList<Iterator, T>::prev()
{
    if (curIt_ == firstItemIt_) {
        // next two lines is "etl::prev(...)" analog
        curIt_ = endItemIt_;
        --curIt_;

        firstIt_ = curIt_;
        decltype(visible_) c = visible();

        // Reason of "--c":
        //   firstIt_ is already on last item, so it's needed to do one step less then "visible_"
        while (firstIt_ != firstItemIt_ && --c > 0) {
            --firstIt_;
        }
    } else {
        if (firstIt_ == curIt_) {
            --firstIt_;
        }
        --curIt_;
    }
}

template<typename Iterator, typename T>
void
UISelectionList<Iterator, T>::update(Iterator start, Iterator end)
{
    // if (Serial) { Serial.println(F("UISelectionList::update(...)")); }

    firstItemIt_ = start;
    endItemIt_ = end;

    firstIt_ = start;
    curIt_ = start;
}

template<typename Iterator, typename T>
void
UISelectionList<Iterator, T>::draw()
{
    // if (Serial) { Serial.println(F("UISelectionList<Iterator, T>::draw()")); }
    oled_.setInverseFont(0);
    oled_.clear();
    oled_.home();

    if (!title_.empty()) {
        oled_.println(title_.c_str());
    }

    decltype(visible_) rowsForItems = visible();
    // if (Serial) {
    //     Serial.print(F("rowsForItems = "));
    //     Serial.println(rowsForItems);
    // }

    // if ( u8sl.current_pos >= u8sl.total )
    //   u8sl.current_pos = u8sl.total-1;

    // if (Serial) {
    //     Serial.print(F("firstIt_ = "));
    //     Serial.println(static_cast<const char*>(*firstIt_));
    //
    //     Serial.print(F("curIt_ = "));
    //     Serial.println(static_cast<const char*>(*curIt_));
    // }

    Iterator it = firstIt_;
    for(uint8_t i = 0; it != endItemIt_ && i < rowsForItems; ++i, ++it) {
        if (it == curIt_) {
            oled_.setInverseFont(1);
        }
        // if (Serial) {
        //     Serial.print(F("Item: "));
        //     Serial.println(static_cast<const char*>(*it));
        // }
        oled_.println(static_cast<const char*>(*it));
        if (it == curIt_) {
            oled_.setInverseFont(0);
        }
    }
}

template<typename Iterator, typename T>
uint8_t
UISelectionList<Iterator, T>::visible() const
{
#if defined(EPOXY_DUINO)
    return oled_.getRows() - (title_.empty() ? 0 : 1);
#else
    // FIXME: Figure out why getRows() return wrong value. Right now 4 rows per screen is hard-coded.
    return 4 - (title_.empty() ? 0 : 1);
#endif
}
