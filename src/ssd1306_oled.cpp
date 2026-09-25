// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#include "ssd1306_oled.hpp"
#include "accounts_menu.hpp"

void
SSD1306I2C::setup()
{
    u8x8_.begin();
}

void
SSD1306I2C::clear()
{
    u8x8_.clearDisplay();
}

void
SSD1306I2C::home()
{
    u8x8_.home();
}

void
SSD1306I2C::setFont(const uint8_t* font)
{
    u8x8_.setFont(font);
}

void
SSD1306I2C::setInverseFont(uint8_t value)
{
    u8x8_.setInverseFont(value);
}

size_t
SSD1306I2C::write(uint8_t b)
{
    return u8x8_.write(b);
}

size_t
SSD1306I2C::write(const uint8_t *buffer, size_t size)
{
    return u8x8_.write(buffer, size);
}

void
SSD1306I2C::display()
{
    u8x8_.display();
}

uint8_t
SSD1306I2C::getRows(void)
{
    return u8x8_.getRows();
}

uint8_t
SSD1306I2C::getCols(void)
{
    return u8x8_.getCols();
}

void
SSD1306I2C::drawUTF8(uint8_t col, uint8_t row, const char *s)
{
    u8x8_.drawUTF8(col, row, s);
}
