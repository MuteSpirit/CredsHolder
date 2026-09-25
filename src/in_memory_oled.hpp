// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
#pragma once

#include "oled.hpp"

// TODO: react on \n and jump to next k*width
template<uint8_t ROWS, uint8_t COLS>
class InMemoryPrint : public Oled
{
    static constexpr size_t memorySize = ROWS * COLS;

public:
    virtual size_t write(uint8_t c) override {
      if (mIndex < memorySize - 1) {
        mBuf_[mIndex] = c;
        mIndex++;
        return 1;
      } else {
        return 0;
      }
    }

    virtual size_t write(const uint8_t *buffer, size_t size) override {
      if (buffer == nullptr) return 0;

      while (size > 0 && mIndex < memorySize - 1) {
        write(*buffer++);
        size--;
      }
      return size;
    }

    virtual void drawUTF8(uint8_t col, uint8_t row, const char *s) override {
        if (col >= COLS) {
            return;
        }

        if (row >= ROWS) {
            return;
        }
        size_t len = strlen(s);
        strncpy(mBuf_ + row * COLS + col, s, len <= COLS - col ? len : COLS - col);
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
      mBuf_[mIndex] = '\0';
      return mBuf_;
    }

     constexpr uint16_t getBufSize() const {
        return memorySize;
    }

    virtual void clear() override {
        memset(mBuf_, 0, memorySize);
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
    virtual void setInverseFont(uint8_t value) override;


    void getLine(const uint8_t idx, char *buf, const uint8_t len) {
        if (idx > ROWS - 1) {
            buf[0] = '\0';
            return;
        }
        memcpy(buf, mBuf_ + idx*COLS, len <= COLS ? len : COLS);
    }

    virtual void display() override
    {
    }

    virtual uint8_t getRows(void) override
    {
        return ROWS;
    }

    virtual uint8_t getCols(void) override
    {
        return COLS;
    }

  private:
    mutable char mBuf_[memorySize];
    uint8_t mIndex = 0;
};
