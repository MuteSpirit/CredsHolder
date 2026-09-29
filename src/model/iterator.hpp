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

#include "../creds_holder.hpp"
#include <inttypes.h>
#include <Embedded_Template_Library.h>
#include <etl/iterator.h>

#include "storage.hpp"

class Account;
template <typename T> class ModelIteratorImpl;


////////////////////////////////////////////////////////////////////////////////
/// Iterator over objects in ModelStorage<T> storage.
///
/// It may be used in UISelectionList
///
/// There is special case when current iterator means "cend"/"end".
/// At that stage idx_ is equal -1.
/// And it's needed to jump to -1 if increment iterator after maxIdx index.
/// And it's needed to handle decrement when idx_ is equal -1 to try iterate backward.
template<typename T>
class ModelIterator : public etl::iterator<etl::bidirectional_iterator_tag, T>
{
public:
    static constexpr typename ModelStorage<T>::ObjIndex END = -1;
    static constexpr typename ModelStorage<T>::ObjIndex FIRST = 0;

public:
    explicit ModelIterator(const ModelStorage<T>& store, typename ModelStorage<T>::ObjIndex idx = 0);
    ModelIterator(const ModelIterator&) = default;

    T& operator*();
    const T& operator*() const;

    ModelIterator& operator++(); // prefix increment
    ModelIterator operator++(int); // postfix increment

    ModelIterator& operator--(); // prefix decrement
    ModelIterator operator--(int); // postfix decrement

    bool operator==(const ModelIterator& rhs) const;
    bool operator!=(const ModelIterator& rhs) const;

protected:
    ModelIteratorImpl<T>* impl();
    const ModelIteratorImpl<T>* impl() const;

protected:
    /// Buffer for implementation class instance.
    /// @details Use static_assert in constructor to check required size at compile time 
    uint8_t impl_[112];
};

