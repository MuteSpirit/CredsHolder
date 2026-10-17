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
#include <cstddef>

class Account;

struct sqlite3_stmt;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace creds_holder {
/**
 * @brief SQL policy mapping Account onto the "accounts" table.
 */
class AccountSqlTraits {
public:
    using ObjIndex = size_t;

public:
    /** @return Name of the backing table ("accounts"). */
    static const char* tableName();

    /** @return CREATE TABLE statement (idempotent). */
    static const char* createTable();

    /** @return Query selecting all ids, sorted ascending. */
    static const char* selectAllIdsQuery();

    /** @return Query selecting one row by id (binds id as ?1). */
    static const char* selectByIdQuery();

    /** @return Query selecting one row by name (binds name as ?1). */
    static const char* selectByKeyQuery();

    /** @return INSERT statement (binds name, url, username, password). */
    static const char* insertQuery();

    /** @return DELETE statement (binds id as ?1). */
    static const char* deleteByIdQuery();

    /**
     * @brief Returns the row id of an account.
     */
    static ObjIndex idOf(const Account& a);

    /**
     * @brief Returns the key value used by get()/remove().
     */
    static const char* keyOf(const Account& a);

    /**
     * @brief Binds an Account to an INSERT statement.
     */
    static void bindInsert(sqlite3_stmt* stmt, const Account& a);

    /**
     * @brief Reads an Account from the current row of a statement.
     */
    static void deserialize(sqlite3_stmt* stmt, Account& a);
};

} // namespace creds_holder
