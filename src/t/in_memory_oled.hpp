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

#include <string.h>
#include "../oled.hpp"

template<uint8_t COLS, uint8_t ROWS>
class OledInMem : public Oled
{
    static_assert(COLS > 0, "virtual OLED must be non zero width (set COLS more then 0)");
    static_assert(ROWS > 0, "virtual OLED must be non zero heigh (set ROWS more then 0)");

    static constexpr size_t memorySize = ROWS * COLS + 1;

public:
    OledInMem();

    virtual void setup() override;

    virtual size_t write(uint8_t c) override;
    virtual size_t write(const uint8_t *buffer, size_t size) override;

    virtual void drawUTF8(uint8_t col, uint8_t row, const char *s) override;

    /// Return the NULL terminated string containing all text printed on virtual OLED screen
    const char* getBuffer() const;
    constexpr size_t getBufSize() const;

    virtual void clear() override;
    virtual void home() override;

    virtual void setFont(const uint8_t* font) override;
    virtual void setInverseFont(uint8_t value) override;

    void getLine(const uint8_t idx, char *buf, const uint8_t len);
    virtual void display() override;

    virtual uint8_t getRows(void) override;
    virtual uint8_t getCols(void) override;

    size_t curPos() const;

protected:
    void incPos();

protected:
    mutable char mBuf_[memorySize]; // +1 for terminal '\0'

    // Separate column and row positions are needed to handle input '\r' and '\n' symbols.
    // With single "curPos_" calculations will be too complex for test
    uint8_t col_ {0}; /// Current column
    uint8_t row_ {0}; /// Current row
};

template<uint8_t COLS, uint8_t ROWS>
size_t
OledInMem<COLS, ROWS>::curPos() const
{
    return row_ * COLS + col_;
}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::incPos()
{
    if (++col_ >= COLS) {
        col_ = 0;
        ++row_; // may become more then ROWS. Let's print nothing after "memorySize" boundary
    }
}

template<uint8_t COLS, uint8_t ROWS>
OledInMem<COLS, ROWS>::OledInMem()
{
    clear();
}

template<uint8_t COLS, uint8_t ROWS>
size_t
OledInMem<COLS, ROWS>::write(uint8_t c)
{
    if (c == '\r') {
        col_ = 0;
        return 1;
    } else if (c == '\n') {
        ++row_;
        return 1;
    } else {
        if (col_ >= COLS
            || row_ >= ROWS
            || curPos() >= memorySize) {
            return 0;
        }

        mBuf_[curPos()] = c;
        ++col_;
        return 1;
    }
}

// TODO: react on \n and jump to next k*width
template<uint8_t COLS, uint8_t ROWS>
size_t
OledInMem<COLS, ROWS>::write(const uint8_t *buffer, size_t size)
{
    if (buffer == nullptr) {
        return 0;
    }

    while (size > 0 && curPos() < memorySize) {
        write(*buffer++);
        size--;
    }
    return size;
}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::drawUTF8(uint8_t col, uint8_t row, const char *s)
{
    if (col >= COLS) {
        return;
    }

    if (row >= ROWS) {
        return;
    }

    uint8_t oldRow = row_;
    uint8_t oldCol = col_;

    row_ = row;
    col_ = col;

    write(reinterpret_cast<const uint8_t*>(s), strlen(s));

    row_ = oldRow;
    col_ = oldCol;
}

template<uint8_t COLS, uint8_t ROWS>
const char*
OledInMem<COLS, ROWS>::getBuffer() const
{
    mBuf_[curPos()] = '\0';
    return mBuf_;
}

template<uint8_t COLS, uint8_t ROWS>
constexpr
size_t
OledInMem<COLS, ROWS>::getBufSize() const
{
    return memorySize;
}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::clear()
{
    memset(mBuf_, 0, memorySize);
}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::home()
{
    row_ = 0;
    col_ = 0;
}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::setup()
{}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::setFont(const uint8_t* font)
{
    (void)font;
};

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::setInverseFont(uint8_t value)
{
    (void)value;
    // TODO
    // Maybe add one more symbol near each letter to mark inversion
    // For example,
    //   regular text: sometext
    //   inverted one: !s!o!m!e!t!e!x!t
}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::getLine(const uint8_t idx, char *buf, const uint8_t len)
{
    if (idx > ROWS - 1) {
        buf[0] = '\0';
        return;
    }
    memcpy(buf, mBuf_ + idx * COLS, len <= COLS ? len : COLS);
}

template<uint8_t COLS, uint8_t ROWS>
void
OledInMem<COLS, ROWS>::display()
{
    // there is no double buffering so nothing to do
}

template<uint8_t COLS, uint8_t ROWS>
uint8_t
OledInMem<COLS, ROWS>::getRows(void)
{
    return ROWS;
}

template<uint8_t COLS, uint8_t ROWS>
uint8_t
OledInMem<COLS, ROWS>::getCols(void)
{
    return COLS;
}
