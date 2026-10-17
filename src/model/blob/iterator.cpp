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
#include "iterator.hpp"

#include <new>
#include "../account.hpp"
#include "storage.hpp"

namespace creds_holder {

////////////////////////////////////////////////////////////////////////////////
template<typename T>
class ModelIteratorImpl
{
public:
    ModelIteratorImpl(const ModelBlobStorage<T> &store, typename ModelBlobStorage<T>::ObjIndex idx);

    ModelIteratorImpl<T>& operator--();
    ModelIteratorImpl<T>& operator++();

public:
    const ModelBlobStorage<T>& store_;

    /// equation to (0)  means begin iterator
    /// equation to (-1) means end   iterator
    typename ModelBlobStorage<T>::ObjIndex idx_ {0};

    /// Keep copy of object:
    /// 1. To avoid separate request to model storage with access external hw storage
    /// 2. To have ability return reference (T&) and pointer (T*) without broken ref/ptr
    T t_;
};


////////////////////////////////////////////////////////////////////////////////
template<typename T>
ModelIterator<T>::ModelIterator(const ModelBlobStorage<T> &store, typename ModelBlobStorage<T>::ObjIndex idx)
{
    static_assert(sizeof(impl_) >= sizeof(ModelIteratorImpl<T>), "fix ModelIterator<T>::impl_ size");
    new (impl_) ModelIteratorImpl<T>(store, idx);
}

template<typename T>
ModelIteratorImpl<T>::ModelIteratorImpl(const ModelBlobStorage<T> &store, typename ModelBlobStorage<T>::ObjIndex idx)
    : store_(store)
    , idx_(idx)
{
    if (ModelIterator<T>::END != idx_) {
        store_.get(idx_, t_) || store_.getNext(idx_, t_, idx_);
    }
}

template<typename T>
ModelIteratorImpl<T>*
ModelIterator<T>::impl()
{
    return reinterpret_cast<ModelIteratorImpl<T>*>(impl_);
}

template<typename T>
const ModelIteratorImpl<T>*
ModelIterator<T>::impl() const
{
    return reinterpret_cast<const ModelIteratorImpl<T>*>(impl_);
}

template<typename T>
T&
ModelIterator<T>::operator*()
{
    return impl()->t_;
}

template<typename T>
const T&
ModelIterator<T>::operator*() const
{
    return impl()->t_;
}

template<typename T>
ModelIterator<T>&
ModelIterator<T>::operator++()
{
    impl()->operator++();
    return *this;
}

template<typename T>
ModelIteratorImpl<T>&
ModelIteratorImpl<T>::operator++()
{
    if (idx_ == store_.maxIdx()
            || (ModelIterator<T>::END != idx_ 
                && !store_.getNext(idx_, t_, idx_))) {
        idx_ = ModelIterator<T>::END;
        t_.reset();
    } 
    return *this;
}

template<typename T>
ModelIterator<T>
ModelIterator<T>::operator++(int)
{
    ModelIterator<T> tmp(*this);
    operator++();
    return tmp;
}

template<typename T>
ModelIterator<T>&
ModelIterator<T>::operator--()
{
    impl()->operator--();
    return *this;
}

template<typename T>
ModelIteratorImpl<T>&
ModelIteratorImpl<T>::operator--()
{
    if (ModelIterator<T>::END == idx_) {
        idx_ = store_.maxIdx();
        if (!store_.get(idx_, t_) 
            && !store_.getPrev(idx_, t_, idx_)) {
            idx_ = ModelIterator<T>::END;
            t_.reset();
        }
    } else if (ModelIterator<T>::FIRST != idx_) {
        if (!store_.getPrev(idx_, t_, idx_)) {
            // FIXME: if there will be empty blocks on some Account positions then
            // current idx_ maybe not equal to FIRST and all [FIRST, idx_] are empty too
            idx_ = ModelIterator<T>::FIRST;
            t_.reset();
        }
    } 
    // else ModelIterator<T>::FIRST == idx_
    // undefined behavior
    // stay on the same index to avoid cyclic iteration
    return *this;
}

template<typename T>
ModelIterator<T>
ModelIterator<T>::operator--(int)
{
    ModelIterator<T> tmp(*this);
    operator--();
    return tmp;
}

template<typename T>
bool
ModelIterator<T>::operator==(const ModelIterator<T>& rhs) const
{
    return &impl()->store_ == &rhs.impl()->store_
        && impl()->idx_ == rhs.impl()->idx_;
}

template<typename T>
bool
ModelIterator<T>::operator!=(const ModelIterator<T>& rhs) const
{
    return not operator==(rhs);
}

////////////////////////////////////////////////////////////////////////////////
// Let's instantiate for Account to keep control on ModelBlobStorage class usage
template class ModelIterator<Account>;

} // namespace creds_holder
