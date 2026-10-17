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
#include "account_traits.hpp"

#include "../../creds_holder.hpp"
#include <cstring>

#if defined(ARDUINO)
#  include <Arduino.h>
#else
#  define PROGMEM
#endif

#include "../../sqlite.hpp"
#include "../account.hpp"


namespace creds_holder {

static const char * sqlTableName PROGMEM = "accounts";
static const char * sqlCreateTable PROGMEM = ""
        "CREATE TABLE IF NOT EXISTS accounts ("
        "  id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name     TEXT NOT NULL,"
        "  url      TEXT,"
        "  username TEXT,"
        "  password TEXT"
        ");";
static const char* sqlSelectAllIdsQuery PROGMEM = "SELECT id FROM accounts ORDER BY id;";
static const char* sqlSelectByIdQuery PROGMEM = "SELECT id, name, url, username, password FROM accounts WHERE id = ?;";
static const char* sqlSelectByKeyQuery PROGMEM = "SELECT id, name, url, username, password FROM accounts WHERE name = ? LIMIT 1;";
static const char* sqlInsertQuery PROGMEM = "INSERT INTO accounts (name, url, username, password) VALUES (?,?,?,?);";
static const char* sqlDeleteByIdQuery PROGMEM = "DELETE FROM accounts WHERE id = ?;";


const char*
AccountSqlTraits::tableName()
{
    return sqlTableName;
}

const char*
AccountSqlTraits::createTable()
{
    return sqlCreateTable;
}

const char*
AccountSqlTraits::selectAllIdsQuery()
{
    return sqlSelectAllIdsQuery;
}

const char*
AccountSqlTraits::selectByIdQuery()
{
    return sqlSelectByIdQuery;
}

const char*
AccountSqlTraits::selectByKeyQuery()
{
    return sqlSelectByKeyQuery;
}

const char*
AccountSqlTraits::insertQuery()
{
    return sqlInsertQuery;
}

const char*
AccountSqlTraits::deleteByIdQuery()
{
    return sqlDeleteByIdQuery;
}

typename AccountSqlTraits::ObjIndex
AccountSqlTraits::idOf(const Account& a) {
    return a.id;
}

const char* AccountSqlTraits::keyOf(const Account& a) {
    return a.name;
}

void AccountSqlTraits::bindInsert(sqlite3_stmt* stmt, const Account& a) {
    sqlite3_bind_text(stmt, 1, a.name,     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, a.url,      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, a.username, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, a.password, -1, SQLITE_TRANSIENT);
}

static void readTextInto(sqlite3_stmt* stmt, int col, char* buffer, size_t maxLen) {
    const unsigned char* text = sqlite3_column_text(stmt, col);
    if (!text) {
        buffer[0] = '\0';
        return;
    }
    const char* src = reinterpret_cast<const char*>(text);
    size_t len = std::strlen(src);
    if (len > maxLen) len = maxLen;
    std::memcpy(buffer, src, len);
    buffer[len] = '\0';
}

void
AccountSqlTraits::deserialize(sqlite3_stmt* stmt, Account& a)
{
    a.id = sqlite3_column_int(stmt, 0);

    readTextInto(stmt, 1, a.name,     Account::nameMaxLen - 1);
    readTextInto(stmt, 2, a.url,      Account::urlMaxLen - 1);
    readTextInto(stmt, 3, a.username, Account::usernameMaxLen - 1);
    readTextInto(stmt, 4, a.password, Account::passwordMaxLen - 1);
}

} // namespace creds_holder
