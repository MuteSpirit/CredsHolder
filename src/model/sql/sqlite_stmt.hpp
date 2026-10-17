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

struct sqlite3;
typedef struct sqlite3 sqlite3;

struct sqlite3_stmt;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace creds_holder {

/**
 * @brief RAII wrapper around a prepared SQLite3 statement.
 */
class SqliteStmt {
public:
    /** @brief Constructs an empty (invalid) statement. */
    SqliteStmt();

    /**
     * @brief Prepares a statement against an open database.
     * @param db  Open sqlite3 handle.
     * @param sql SQL text to prepare.
     */
    SqliteStmt(sqlite3* db, const char* sql);

    /** @brief Finalizes the underlying statement if owned. */
    ~SqliteStmt();

    SqliteStmt(const SqliteStmt&) = delete;
    SqliteStmt& operator=(const SqliteStmt&) = delete;

    /** @brief Move constructor. */
    SqliteStmt(SqliteStmt&& other);

    /** @brief Move assignment. */
    SqliteStmt& operator=(SqliteStmt&& other);

    /** @return Raw sqlite3_stmt pointer (may be nullptr). */
    sqlite3_stmt* get() const;

    /** @return true if the statement was prepared successfully. */
    explicit operator bool() const;

    /** @brief Finalizes and releases the underlying statement. */
    void reset();

private:
    sqlite3_stmt* stmt;
};

} // namespace creds_holder
