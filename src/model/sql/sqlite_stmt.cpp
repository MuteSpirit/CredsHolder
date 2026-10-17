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
#include "sqlite_stmt.hpp"
#include "../../sqlite.hpp"


namespace creds_holder {

SqliteStmt::SqliteStmt()
    : stmt(nullptr) {}

SqliteStmt::SqliteStmt(sqlite3* db, const char* sql)
    : stmt(nullptr)
{
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        stmt = nullptr;
    }
}

SqliteStmt::~SqliteStmt() {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

SqliteStmt::SqliteStmt(SqliteStmt&& other)
    : stmt(other.stmt)
{
    other.stmt = nullptr;
}

SqliteStmt& SqliteStmt::operator=(SqliteStmt&& other)
{
    if (this != &other) {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
        stmt = other.stmt;
        other.stmt = nullptr;
    }
    return *this;
}

sqlite3_stmt* SqliteStmt::get() const { return stmt; }

SqliteStmt::operator bool() const { return stmt != nullptr; }

void SqliteStmt::reset() {
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
}

} // namespace creds_holder
