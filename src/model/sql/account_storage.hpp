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

// Include files here instead of forward declaration to avoid includes duplication
// in all files which would like to use AccountSqlStorage
#include "../account.hpp"
#include "account_traits.hpp"
#include "storage.hpp"

namespace creds_holder {

/** @brief Maximum accounts to store. (16 - only during debug/MVP) */
static const size_t maxStoreAccounts = 16;

/** @brief Alias: Account-specialized generic storage. */
using AccountSqlStorage = ModelSqlStorage<Account, AccountSqlTraits, maxStoreAccounts>;


} // namespace creds_holder
