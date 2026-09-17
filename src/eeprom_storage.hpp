#pragma once

#include <inttypes.h>

// #if defined(ARDUINO_ARCH_NRF52)
// #ifdef USE_TINYUSB
// #undef USE_TINYUSB
// #endif
// #endif

#if defined(ARDUINO_ARCH_NRF52)
#define NRF52840
#  if !defined(USE_TINYUSB)
#  define USE_TINYUSB 1
#  endif
#endif
#include <Arduino.h>
#include <EEPROM_SPI_WE.h>

#include "block_storage.hpp"

// TODO: make "EEPROM write protect pin manageble and optimize bulk write operations"
// TODO: make [minAddr, maxAddr] diapasone configurable to store different model objects in different non-overlapped diapasones

class EepromStorage : public BlockStorage
{
public:
    EepromStorage(const uint8_t csPin, const unsigned int wpPin = 999, const unsigned int speed = F_CPU);
    ~EepromStorage() = default;

    bool init(const enum eeprom_size_t fullSizeBytes, const enum EEPROM_WE_PAGE_SIZE pageSize);

public:
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
    EEPROM_SPI_WE eep_;
    uint32_t fullSizeBytes_;
    uint16_t pageSize_;
};
