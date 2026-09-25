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
