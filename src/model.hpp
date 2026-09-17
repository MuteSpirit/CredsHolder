#pragma once
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <inttypes.h>
#include <stdio.h>
#include <stddef.h>

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

#endif // !__MODEL_HPP__
