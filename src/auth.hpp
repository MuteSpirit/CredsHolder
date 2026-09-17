#pragma once
#ifndef __AUTH_HPP__
#define __AUTH_HPP__

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

#endif // !__AUTH_HPP__
