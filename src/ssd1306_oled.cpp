// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#include <cstddef>

#include "ssd1306_oled.hpp"

void
SSD1306I2C::setup()
{
    u8g2_.begin();
}

void
SSD1306I2C::clear()
{
    u8g2_.clearDisplay();
}

void
SSD1306I2C::home()
{
    u8g2_.home();
}

void
SSD1306I2C::setFont(const uint8_t* font)
{
    u8g2_.setFont(font);
}

size_t
SSD1306I2C::write(uint8_t b)
{
    return u8g2_.write(b);
}

size_t
SSD1306I2C::write(const uint8_t *buffer, size_t size)
{
    return u8g2_.write(buffer, size);
}
