// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once
#include "creds_holder.hpp"

#include "etl/iterator.h"
#include "etl/memory.h"
#include "etl/string.h"
#include <inttypes.h>

#include "oled.hpp"

class UISelectionListTestHelper;


template<typename Iterator, typename T>
class UISelectionList
{
public:
    UISelectionList(Oled& oled, const char* title, Iterator start, Iterator end);

    const T& selected() const;

    void next();
    void prev();

    void draw();

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
    , visible_(oled.getRows())
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
        if (etl::distance(firstIt_, curIt_) > visible_) {
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
        decltype(visible_) c = visible_;

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
UISelectionList<Iterator, T>::draw()
{
    oled_.setInverseFont(0);
    oled_.home();
    oled_.clear();

    decltype(visible_) visible = visible_;

    if (!title_.empty()) {
        oled_.println(title_.c_str());
        --visible;
    }

    // if ( u8sl.current_pos >= u8sl.total )
    //   u8sl.current_pos = u8sl.total-1;

    Iterator it = firstIt_;
    for(uint8_t i = 0; it != endItemIt_ && i < visible; ++i, ++it) {
        if (it == curIt_) {
            oled_.setInverseFont(1);
        }
        oled_.println(static_cast<const char*>(*it));
        if (it == curIt_) {
            oled_.setInverseFont(0);
        }
    }
}
