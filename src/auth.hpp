// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

#include <inttypes.h>

////////////////////////////////////////////////////////////////////////////////
/// @brief Allow to verify password
class Authenticator
{
public:
    virtual bool auth(const char *password, const uint8_t len) = 0;

    virtual ~Authenticator() = default;
};

////////////////////////////////////////////////////////////////////////////////
/// Accept salt and hash in ctor to hide that internals behind Authenticator 
/// interface
class PasswordWandAuth : public Authenticator
{
public:
    static constexpr uint8_t hashSize = 32; // == sha256 hash size
        
public:
    PasswordWandAuth();

    void init(const char *salt, const uint8_t saltSize,
              const uint8_t *saltedPasswordHash, const uint8_t saltedPasswordHashSize);
    /// Max supported password length is "hashSize", the rest tail will be cut.
    virtual bool auth(const char *password, const uint8_t len) override;

protected:
    /// Use salt the same max length as password to allow password to be short 
    /// with keeping broutforce complexity for EEPROM data decryption
    /// Salt will be randomly generated and will be enough long.
    char salt_[hashSize];
    uint8_t saltedPasswordHash_[hashSize];
};
