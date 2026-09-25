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

#include <inttypes.h>
#include <stdio.h>
#include <cstddef>

// TODO: use enum ?
#define ACCOUNT_NAME_SIZE 32
#define USERNAME_SIZE     32
#define PASSWORD_SIZE     32

struct __attribute__((packed)) Account
{
    char name[ACCOUNT_NAME_SIZE];
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
};

template<typename Object>
char *get_key_ptr(Object &o);

template<typename Object>
uint8_t get_key_size();

template<typename Object>
ptrdiff_t get_key_offset();
