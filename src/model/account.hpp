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
#include "../creds_holder.hpp"

#include <cstddef>
#include <cstdint>


/**
 * @brief Represents a single credential account.
 *
 * A plain data-holder used as the model type stored in
 * creds_holder::ModelStorage. Fields are public by design.
 */

class  __attribute__((packed)) Account {
public:
    using ObjIndex = size_t;

    static constexpr const size_t nameMaxLen     = 32;
    static constexpr const size_t urlMaxLen      = 256;
    static constexpr const size_t usernameMaxLen = 32;
    static constexpr const size_t passwordMaxLen = 32;

    static constexpr const size_t keyMaxLen = nameMaxLen;

public:
    /** @brief Default-constructs an empty Account with id = 0. */
    Account();

    /**
     * @brief Constructs a fully-initialized Account.
     * @param idVal       Row id (0 for new records).
     * @param nameVal     Account name.
     * @param urlVal      Account URL.
     * @param usernameVal Account username.
     * @param passwordVal Account password.
     */
    Account(size_t idVal,
            const char* nameVal,
            const char* urlVal,
            const char* usernameVal,
            const char* passwordVal);

    operator const char*();

    void reset();

public:
    /** @brief Account row id (0 when not yet persisted). */
    ObjIndex id {0};

    /** @brief Human-readable account name (also the lookup key). */
    char name[nameMaxLen];

    /** @brief Account URL (may be empty). */
    char url[urlMaxLen];

    /** @brief Account username (may be empty). */
    char username[usernameMaxLen];

    /** @brief Account password (may be empty). */
    char password[passwordMaxLen];
};

bool operator==(const Account& lhs, const Account& rhs);

template<typename Object>
char *get_key_ptr(Object &o);
 
template<typename Object>
uint8_t get_key_size();
 
template<typename Object>
ptrdiff_t get_key_offset();
