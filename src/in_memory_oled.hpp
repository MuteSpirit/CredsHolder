#pragma once
#ifndef __IN_MEMORY_OLED_HPP__
#define __IN_MEMORY_OLED_HPP__

#include "oled.hpp"

// TODO: react on \n and jump to next k*width
template<uint8_t height, uint8_t width>
class InMemoryPrint : public Oled {
    static constexpr uint16_t memorySize = height * width;

  public:
    size_t write(uint8_t c) override {
      if (mIndex < memorySize - 1) {
        mBuf[mIndex] = c;
        mIndex++;
        return 1;
      } else {
        return 0;
      }
    }

    size_t write(const uint8_t *buffer, size_t size) override {
      if (buffer == nullptr) return 0;

      while (size > 0 && mIndex < memorySize - 1) {
        write(*buffer++);
        size--;
      }
      return size;
    }

// ESP32 and STM32duino do not provide a virtual Print::flush() method.
#if defined(ESP32) || defined(ARDUINO_ARCH_STM32)
    void flush() {
#else
    void flush() override {
#endif
      mIndex = 0;
    }

    /**
     * Return the NUL terminated string buffer. After the buffer is no longer
     * needed, the flush() method should be called to reset the internal buffer
     * index to 0.
     */
    const char* getBuffer() const {
      mBuf[mIndex] = '\0';
      return mBuf;
    }

     constexpr uint16_t getBufSize() const {
        return memorySize;
    }

    virtual void clear() override {
        memset(mBuf, 0, memorySize);
    }

    virtual void home() override {
        mIndex = 0;
    }

    virtual void setup() override
    {} 

    virtual void setFont(const uint8_t* font) override
    {
        (void)font;
    };

    void getLine(const uint8_t idx, char *buf, const uint8_t len) {
        if (idx > height - 1) {
            buf[0] = '\0';
            return;
        }
        memcpy(buf, mBuf + idx*width, len < width ? len : width);
    }

  private:
    mutable char mBuf[memorySize];
    uint8_t mIndex = 0;
};

#endif // !__IN_MEMORY_OLED_HPP__
