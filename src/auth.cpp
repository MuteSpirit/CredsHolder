#include "auth.hpp"
#include <string.h>
#include <SHA256.h>

PasswordWandAuth::PasswordWandAuth()
{
    memset(salt_, 0, hashSize);
    memset(saltedPasswordHash_, 0, hashSize);
}

void
PasswordWandAuth::init(const char *salt, const uint8_t saltSize,
                       const uint8_t *saltedPasswordHash, const uint8_t saltedPasswordHashSize)
{
    strncpy(salt_, salt, saltSize <= hashSize ? saltSize : hashSize);
    memcpy(saltedPasswordHash_, saltedPasswordHash, saltedPasswordHashSize <= hashSize ? saltedPasswordHashSize : hashSize);
}

bool
PasswordWandAuth::auth(const char *password, const uint8_t len)
{
    char saltedPassword[hashSize * 2];
    memset(saltedPassword, 0, sizeof(saltedPassword));

    const uint8_t usedPassLen = len <= hashSize ? len : hashSize;
    strncpy(saltedPassword, password, usedPassLen);
    strncpy(saltedPassword + usedPassLen, salt_, hashSize * 2 - usedPassLen);

    uint8_t hash[hashSize];
    memset(hash, 0, sizeof(hash));

    SHA256 hasher;
    hasher.reset();
    hasher.update(saltedPassword, strlen(saltedPassword));
    hasher.finalize(hash, PasswordWandAuth::hashSize);

    return !memcmp(saltedPasswordHash_, hash, hashSize);
}
