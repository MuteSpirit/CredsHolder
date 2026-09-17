#ifndef __EEPROM_STORAGE__
#define __EEPROM_STORAGE__

#include <inttypes.h>

#include "block_storage.hpp"

class InternalEepromStorage : public BlockStorage
{
public:
    InternalEepromStorage();
    ~InternalEepromStorage() = default;
    bool init();

    virtual size_t minAddr() const override;
    virtual size_t maxAddr() const override;

    virtual bool isAddrOk(const size_t addr) const override;

    virtual uint8_t read(const size_t addr) override;
    virtual void write(const size_t addr, const uint8_t b) override;

    virtual void read(const size_t addr, uint8_t *buf, const size_t size) override;
    virtual void write(const size_t addr, const uint8_t *buf, const size_t size) override;

    virtual void factoryReset() override;

protected:
    uint8_t getResetFlag();
    void setResetFlag();

protected:
    unsigned int fullSizeBytes_; // TODO: is it needed to cache that value or use EEPROM.length() directly?
};

#endif // !__EEPROM_STORAGE__

