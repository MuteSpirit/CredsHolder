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
#include "../../creds_holder.hpp"
#include <cstddef>
#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/iterator.h>

#include "../../sqlite.hpp"
#include "sqlite_stmt.hpp"


namespace creds_holder {

/**
 * @brief Generic SQLite-backed storage for model type T.
 *
 * @tparam T         Model type (default-constructible & copyable).
 * @tparam SqlTraits Policy class describing how T maps onto SQL.
 * @tparam MaxItems  Compile-time capacity of the id cache.
 */
template <typename T, typename SqlTraits, size_t MaxItems>
class ModelSqlStorage {
public:
    using ObjIndex = typename T::ObjIndex;

    /**
     * @brief Bidirectional const iterator over cached sorted ids.
     */
    class ConstIterator {
    public:
        typedef etl::bidirectional_iterator_tag iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;

        typedef T*                              pointer;
        typedef const T*                        const_pointer;

        typedef T&                              reference;
        typedef const T&                        const_reference;

        typedef typename ModelSqlStorage::ObjIndex ObjIndex;

        /** @brief Constructs an end-style iterator. */
        ConstIterator();

        /**
         * @brief Constructs an iterator at a given index in the id cache.
         */
        ConstIterator(const ModelSqlStorage* storage, size_t index, bool atEnd);

        const_reference operator*() const;
        reference operator*();

        const_pointer operator->() const;
        pointer operator->();

        /** @brief Pre-increment. */
        ConstIterator& operator++();

        /** @brief Post-increment. */
        ConstIterator operator++(int);

        /** @brief Pre-decrement (bidirectional). */
        ConstIterator& operator--();

        /** @brief Post-decrement (bidirectional). */
        ConstIterator operator--(int);

        /** @brief Equality comparison. */
        bool operator==(const ConstIterator& other) const;

        /** @brief Inequality comparison. */
        bool operator!=(const ConstIterator& other) const;

    private:
        void loadCurrent();

        const ModelSqlStorage*  storage;
        typename T::ObjIndex index;
        bool                 atEnd;
        T                    currentObj;
    }; // class ConstIterator

    /**
     * @brief Constructs storage bound to an open SQLite handle.
     */
    explicit ModelSqlStorage();

    void init(sqlite3* db);
    /**
     * @brief Creates the backing table if it does not exist.
     */
    bool initSchema();

    /**
     * @brief Inserts a new object into the table.
     */
    bool add(const T& obj);

    /**
     * @brief Retrieves an object by its key field.
     */
    bool get(const char* key, T& out) const;

    /**
     * @brief Retrieves an object by its key field.
     */
    bool isExist(const char* key) const;

    /**
     * @brief Deletes an object by its key field.
     */
    bool remove(const char* key);

    /** @return Const iterator to the first element. */
    ConstIterator cbegin() const;

    /** @return Const iterator past the last element. */
    ConstIterator cend() const;

private:
    bool loadById(ObjIndex id, T& out) const;
    bool loadByKey(const char* key, T& out) const;

    void refreshCache();
    void insertIdSorted(ObjIndex id);
    void removeIdFromCache(ObjIndex id);

private:
    sqlite3*                   db {nullptr};
    etl::vector<ObjIndex, MaxItems> idCache;
};


// ============================================================
// ConstIterator
// ============================================================

template <typename T, typename SqlTraits, size_t MaxItems>
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::ConstIterator()
    : storage(nullptr), index(0), atEnd(true) {}

template <typename T, typename SqlTraits, size_t MaxItems>
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::ConstIterator(
        const ModelSqlStorage* storagePtr, size_t indexVal, bool atEndVal)
    : storage(storagePtr), index(indexVal), atEnd(atEndVal) {
    if (!atEnd) {
        loadCurrent();
    }
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::const_reference
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator*() const {
    return currentObj;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::reference
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator*() {
    return currentObj;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::const_pointer
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator->() const {
    return &currentObj;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::pointer
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator->() {
    return &currentObj;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator&
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator++() {
    if (!atEnd) {
        if (index + 1 >= storage->idCache.size()) {
            atEnd = true;
            index = storage->idCache.size();
        } else {
            ++index;
            loadCurrent();
        }
    }
    return *this;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator++(int) {
    ConstIterator tmp = *this;
    ++(*this);
    return tmp;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator&
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator--() {
    if (atEnd) {
        if (storage && storage->idCache.size() > 0) {
            index = storage->idCache.size() - 1;
            atEnd = false;
            loadCurrent();
        }
    } else if (index > 0) {
        --index;
        loadCurrent();
    }
    return *this;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator
ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator--(int) {
    ConstIterator tmp = *this;
    --(*this);
    return tmp;
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator==(
        const ConstIterator& other) const {
    if (atEnd && other.atEnd) return true;
    if (atEnd || other.atEnd) return false;
    return index == other.index && storage == other.storage;
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::operator!=(
        const ConstIterator& other) const {
    return !(*this == other);
}

template <typename T, typename SqlTraits, size_t MaxItems>
void ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator::loadCurrent() {
    if (!storage) {
        atEnd = true;
        return;
    }
    ObjIndex id = storage->idCache[index];
    T obj;
    if (storage->loadById(id, obj)) {
        currentObj = obj;
        atEnd = false;
    } else {
        atEnd = true;
    }
}

// ============================================================
// ModelSqlStorage
// ============================================================

template <typename T, typename SqlTraits, size_t MaxItems>
ModelSqlStorage<T, SqlTraits, MaxItems>::ModelSqlStorage()
{
}

template <typename T, typename SqlTraits, size_t MaxItems>
void
ModelSqlStorage<T, SqlTraits, MaxItems>::init(sqlite3* dbPtr)
{
    db = dbPtr;
    refreshCache();
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool
ModelSqlStorage<T, SqlTraits, MaxItems>::initSchema() {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, SqlTraits::createTable(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (Serial) { Serial.println(errMsg); }
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool
ModelSqlStorage<T, SqlTraits, MaxItems>::add(const T& obj) {
    SqliteStmt stmt(db, SqlTraits::insertQuery());
    if (!stmt) {
        if (Serial) { Serial.println(F("ModelSqlStorage::add fail stmt ctor")); }
        return false;
    }

    SqlTraits::bindInsert(stmt.get(), obj);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        if (Serial) { Serial.println(F("ModelSqlStorage::add fail SQL step")); }
        return false;
    }

    int newId = static_cast<int>(sqlite3_last_insert_rowid(db));
    insertIdSorted(newId);

    if (Serial) { Serial.println(F("ModelSqlStorage::add success")); }
    return true;
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool
ModelSqlStorage<T, SqlTraits, MaxItems>::get(const char* key, T& out) const {
    return loadByKey(key, out);
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool
ModelSqlStorage<T, SqlTraits, MaxItems>::isExist(const char* key) const
{
    if (!key) return false;

    SqliteStmt stmt(db, SqlTraits::selectByKeyQuery());

    return static_cast<bool>(stmt);
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool
ModelSqlStorage<T, SqlTraits, MaxItems>::remove(const char* key) {
    if (!key) return false;

    T tmp;
    if (!loadByKey(key, tmp)) {
        return false;
    }
    ObjIndex idToRemove = SqlTraits::idOf(tmp);

    SqliteStmt stmt(db, SqlTraits::deleteByIdQuery());
    if (!stmt) return false;

    sqlite3_bind_int(stmt.get(), 1, idToRemove);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        return false;
    }

    removeIdFromCache(idToRemove);
    return true;
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator
ModelSqlStorage<T, SqlTraits, MaxItems>::cbegin() const {
    if (idCache.empty()) {
        return ConstIterator(this, 0, true);
    }
    return ConstIterator(this, 0, false);
}

template <typename T, typename SqlTraits, size_t MaxItems>
typename ModelSqlStorage<T, SqlTraits, MaxItems>::ConstIterator
ModelSqlStorage<T, SqlTraits, MaxItems>::cend() const {
    return ConstIterator(this, idCache.size(), true);
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool ModelSqlStorage<T, SqlTraits, MaxItems>::loadById(ObjIndex id, T& out) const {
    SqliteStmt stmt(db, SqlTraits::selectByIdQuery());
    if (!stmt) return false;

    sqlite3_bind_int(stmt.get(), 1, id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        SqlTraits::deserialize(stmt.get(), out);
        return true;
    }
    return false;
}

template <typename T, typename SqlTraits, size_t MaxItems>
bool ModelSqlStorage<T, SqlTraits, MaxItems>::loadByKey(const char* key, T& out) const {
    if (!key) return false;

    SqliteStmt stmt(db, SqlTraits::selectByKeyQuery());
    if (!stmt) return false;

    sqlite3_bind_text(stmt.get(), 1, key, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        SqlTraits::deserialize(stmt.get(), out);
        if (Serial) { Serial.println(F("ModelSqlStorage::loadByKey success")); }
        return true;
    }

    if (Serial) { Serial.println(F("ModelSqlStorage::loadByKey sqlite3_step fail")); }
    return false;
}

template <typename T, typename SqlTraits, size_t MaxItems>
void ModelSqlStorage<T, SqlTraits, MaxItems>::refreshCache() {
    idCache.clear();

    SqliteStmt stmt(db, SqlTraits::selectAllIdsQuery());
    if (!stmt) return;

    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        if (idCache.full()) break;
        idCache.push_back(sqlite3_column_int(stmt.get(), 0));
    }
}

template <typename T, typename SqlTraits, size_t MaxItems>
void ModelSqlStorage<T, SqlTraits, MaxItems>::insertIdSorted(ObjIndex id) {
    if (idCache.full()) {
        refreshCache();
        return;
    }

    size_t pos = 0;
    while (pos < idCache.size() && idCache[pos] < id) {
        ++pos;
    }

    idCache.insert(idCache.begin() + pos, id);
}

template <typename T, typename SqlTraits, size_t MaxItems>
void ModelSqlStorage<T, SqlTraits, MaxItems>::removeIdFromCache(ObjIndex id) {
    for (size_t i = 0; i < idCache.size(); ++i) {
        if (idCache[i] == id) {
            idCache.erase(idCache.begin() + i);
            return;
        }
    }
}

} // namespace creds_holder
