
#include "creds_holder.hpp"
#include <EEPROM.h>

#include "internal_eeprom_storage.hpp"

// from Arduino/Crypto docs:
// The random number generator uses 48 bytes of space at the end of EEPROM memory to store the previous seed. When the system is started next time, the previous saved seed is loaded and then deliberately overwritten with a new seed. This ensures that the device will not accidentally generate the same sequence of random numbers if it is restarted before the first automatic save of the seed.
// By default the seed is saved once an hour, although this can be changed with RNG.setAutoSaveTime(). Because the device may be restarted before the first hour expires, there is a special case in the code: the first time that the entropy pool fills up, a save will be automatically forced.
// So the last 48 bytes are busy

#define RESET_FLAG_ADDR (fullSizeBytes_ - 65)
#define RESET_FLAG (0xff)
#define EMPTY_BYTE_VALUE (0x0)

InternalEepromStorage::InternalEepromStorage()
{}

bool
InternalEepromStorage::init()
{
    fullSizeBytes_ = EEPROM.length();

    if (RESET_FLAG != getResetFlag()) {
        factoryReset();
    }

    return true;
}

uint8_t
InternalEepromStorage::getResetFlag()
{
    return EEPROM.read(RESET_FLAG_ADDR);
}

void
InternalEepromStorage::setResetFlag()
{
    EEPROM.write(RESET_FLAG_ADDR, RESET_FLAG);
}

size_t
InternalEepromStorage::minAddr() const
{
    return 0;
}

size_t
InternalEepromStorage::maxAddr() const
{
    return RESET_FLAG_ADDR - 1 /* reset flag size */;
}

bool
InternalEepromStorage::isAddrOk(const size_t addr) const
{
    return minAddr() <= addr && addr < maxAddr();
}

uint8_t
InternalEepromStorage::read(const size_t addr)
{
    return EEPROM.read(addr);
}

void
InternalEepromStorage::write(const size_t addr, const uint8_t b)
{
    if (isAddrOk(addr)) {
        EEPROM.update(addr, b);
    }
}

void
InternalEepromStorage::read(const size_t addr, uint8_t *buf, const size_t size)
{
    for (uint8_t i = 0; i < size && isAddrOk(addr + i); ++i) {
        buf[i] = EEPROM.read(addr + i);
    }
}

void
InternalEepromStorage::write(const size_t addr, const uint8_t *buf, const size_t size)
{
    for (uint8_t i = 0; i < size && isAddrOk(addr + i); ++i) {
        EEPROM.update(addr + i, buf[i]);
    }
}

void
InternalEepromStorage::factoryReset()
{
    for (unsigned int i = 0; i < fullSizeBytes_; ++i) {
        EEPROM.write(i, EMPTY_BYTE_VALUE);
    }
    setResetFlag();
}

