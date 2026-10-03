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


// Snake game code is base on project: https://github.com/aydakikio/arduino_snake
//
// MIT License
//
// Copyright (c) 2025 aydakikio
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include "snake_game.hpp"

#include "creds_holder.hpp"

#include <new>
#include <Arduino.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <U8g2lib.h>

#include "oled.hpp"
#include "device_inputs.hpp"


////////////////////////////////////////////////////////////////////////////////
#define GRID_SIZE 6
#define SCREEN_WIDTH 126
#define SCREEN_HEIGHT 54

// Max snake length
#define SNAKE_MAX 150

#define DRAW_INTERVAL 42

////////////////////////////////////////////////////////////////////////////////
// Store constant strings in PROGMEM (flash memory) to save RAM
const char str_score[] PROGMEM = "Score:";
const char str_best[] PROGMEM = "Best:";
const char str_you_win[] PROGMEM = "YOU WIN!";
const char str_max_length[] PROGMEM = "Max length!";
const char str_game_over[] PROGMEM = "GAME OVER";
const char str_new_high[] PROGMEM = "NEW HIGH SCORE!";
const char str_press_btn[] PROGMEM = "Make any tilt...";

////////////////////////////////////////////////////////////////////////////////
class SnakeGameImpl
{
public:
    struct Point {
        int8_t x, y;
    };

public:
    SnakeGameImpl(Oled&, DeviceInputs&);

    void init(BlindCall nextMenuCb, BlindCall prevMenuCb);

    void activate();
    void deactivate();

    bool setup();
    void loop_step();

    void drawGame();
    void drawGameOver();

    void generateFood();
    void resetHighScore();

    void moveSnake();
    void checkCollisions();
    void growSnake();
    void resetGame();

    void up();
    void down();
    void left();
    void right();
    void reset();

public:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

    Oled& oled_;
    DeviceInputs& userInputs_;
    BlindCall nextMenuCb_;
    BlindCall prevMenuCb_;
    bool activated {false};

    Point snake[SNAKE_MAX];
    int snake_length {3};
    Point food;

    bool input_detected {false};

    int8_t current_direction {0};
    int8_t next_direction {0};
    bool is_direction_changed {false};

    int score {0};
    int high_score {0};
    bool game_over {false};
    bool food_eaten {false};
    bool new_high_score {false};

    unsigned long last_move {0};
    unsigned long last_input {0};
    unsigned long last_draw {0};
    unsigned long game_speed {500};
    unsigned long min_speed {100};

    // Buffer for reading strings from PROGMEM
    char buffer[20];

    // Track if we need to redraw
    bool needs_redraw {true};
};

// TODO: restore keeping results in EEPROM in future versions
// #include <EEPROM.h>
// // EEPROM addresses
// #define EEPROM_HIGH_SCORE_ADDR 0
// #define EEPROM_MAGIC_ADDR 2
// #define EEPROM_MAGIC_VALUE 42

////////////////////////////////////////////////////////////////////////////////
SnakeGameImpl::SnakeGameImpl(Oled& oled, DeviceInputs& userInputs)
    : u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE)
    , oled_(oled)
    , userInputs_(userInputs)
{
    memset(snake, 0, sizeof(snake));
    memset(buffer, 0, sizeof(buffer));
}

void
SnakeGameImpl::init(BlindCall nextMenuCb, BlindCall prevMenuCb)
{
    nextMenuCb_ = nextMenuCb;
    prevMenuCb_ = prevMenuCb;
}

void
SnakeGameImpl::activate()
{
    userInputs_.set(DeviceInputs::UserAction::left, BlindCall::make(this, &SnakeGameImpl::left));
    userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(this, &SnakeGameImpl::right));
    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &SnakeGameImpl::up));
    userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(this, &SnakeGameImpl::down));
    userInputs_.set(DeviceInputs::UserAction::enter, BlindCall::make(this, &SnakeGameImpl::reset));

    activated = true;
}

void
SnakeGameImpl::deactivate()
{
    activated = false;

    userInputs_.unset(DeviceInputs::UserAction::left);
    userInputs_.unset(DeviceInputs::UserAction::right);
    userInputs_.unset(DeviceInputs::UserAction::up);
    userInputs_.unset(DeviceInputs::UserAction::down);
    userInputs_.unset(DeviceInputs::UserAction::enter);
}

bool
SnakeGameImpl::setup() {
    u8g2.begin();

    // TODO: restore keeping results in EEPROM in future versions
    // loadHighScore();

    snake[0].x = GRID_SIZE * 10;
    snake[0].y = 8 + GRID_SIZE * 4;

    snake[1].x = GRID_SIZE * 9;
    snake[1].y = 8 + GRID_SIZE * 4;

    snake[2].x = GRID_SIZE * 8;
    snake[2].y = 8 + GRID_SIZE * 4;

    generateFood();

    // TODO: use TNRG
    randomSeed(analogRead(A2));

    return true;
}

void
SnakeGameImpl::up()
{
    if (game_over) {
        resetGame();
        needs_redraw = true;
        delay(500);
    }

    if (current_direction != 3) {
        next_direction = 2;
        is_direction_changed = true;
        input_detected = true;
        last_input = millis();
    }
}

void
SnakeGameImpl::down()
{
    if (game_over) {
        resetGame();
        needs_redraw = true;
        delay(500);
    }

    if (current_direction != 2) {
        next_direction = 3;
        is_direction_changed = true;
        input_detected = true;
        last_input = millis();
    }
}

void
SnakeGameImpl::left()
{
    if (game_over) {
        resetGame();
        needs_redraw = true;
        delay(500);
    }

    if (current_direction != 0) {
        next_direction = 1;
        is_direction_changed = true;
        input_detected = true;
        last_input = millis();
    }
}

void
SnakeGameImpl::right()
{
    if (game_over) {
        resetGame();
        needs_redraw = true;
        delay(500);
    }

    if (current_direction != 1) {
        next_direction = 0;
        is_direction_changed = true;
        input_detected = true;
        last_input = millis();
    }
}

void
SnakeGameImpl::reset()
{
    resetHighScore();
    needs_redraw = true;
    delay(500);
}

void
SnakeGameImpl::loop_step() {
    if (!activated) {
        return;
    }

    unsigned long current_time = millis();

    if (game_over) {
        // Draw game over screen at fixed rate
        if (current_time - last_draw > DRAW_INTERVAL) {
            drawGameOver();
            last_draw = current_time;
        }
        return;
    }

    // Move snake at game speed
    if (current_time - last_move > game_speed) {
        moveSnake();
        checkCollisions();
        if (food_eaten) {
            growSnake();
            generateFood();
            food_eaten = false;
            score++;

            if (score > high_score) {
                high_score = score;
                new_high_score = true;
                // saveHighScore();
            }

            // Speed progression
            if (score % 5 == 0 && game_speed > min_speed) {
                game_speed -= 15;
                if (game_speed < min_speed) {
                    game_speed = min_speed;
                }
            }

            // Win - reached max length!
            if (snake_length >= SNAKE_MAX) {
                game_over = true;
            }
        }
        needs_redraw = true;
        last_move = current_time;
    }

    // Draw at fixed frame rate
    if (needs_redraw && (current_time - last_draw > DRAW_INTERVAL)) {
        drawGame();
        needs_redraw = false;
        last_draw = current_time;
    }
}

void
SnakeGameImpl::moveSnake() {
    if (is_direction_changed) {
        current_direction = next_direction;
        is_direction_changed = false;
    }

    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    switch (current_direction) {
        case 0: snake[0].x += GRID_SIZE; break;
        case 1: snake[0].x -= GRID_SIZE; break;
        case 2: snake[0].y -= GRID_SIZE; break;
        case 3: snake[0].y += GRID_SIZE; break;
    }
}

void
SnakeGameImpl::checkCollisions() {
    if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH ||
            snake[0].y < 8 || snake[0].y >= SCREEN_HEIGHT + 8) {
        game_over = true;
        return;
    }

    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = true;
            return;
        }
    }

    if (snake[0].x == food.x && snake[0].y == food.y) {
        food_eaten = true;
    }
}

void
SnakeGameImpl::growSnake() {
    if (snake_length < SNAKE_MAX) {
        snake[snake_length] = snake[snake_length - 1];
        snake_length++;
    }
}

void
SnakeGameImpl::generateFood() {
    if (snake_length >= SNAKE_MAX) {
        return;
    }

    bool valid_position = false;

    while (!valid_position) {
        food.x = (random(0, SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
        food.y = 8 + (random(0, SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;

        valid_position = true;
        for (int i = 0; i < snake_length; i++) {
            if (food.x == snake[i].x && food.y == snake[i].y) {
                valid_position = false;
                break;
            }
        }
    }
}

// TODO: restore keeping results in EEPROM in future versions
// void loadHighScore() {
//     byte magic = EEPROM.read(EEPROM_MAGIC_ADDR);
//
//     if (magic != EEPROM_MAGIC_VALUE) {
//         high_score = 0;
//         EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
//         EEPROM.put(EEPROM_HIGH_SCORE_ADDR, high_score);
//     } else {
//         EEPROM.get(EEPROM_HIGH_SCORE_ADDR, high_score);
//     }
// }
//
// void saveHighScore() {
//     EEPROM.put(EEPROM_HIGH_SCORE_ADDR, high_score);
// }

void
SnakeGameImpl::resetHighScore() {
    high_score = 0;
    new_high_score = false;
// TODO: restore keeping results in EEPROM in future versions
    // EEPROM.put(EEPROM_HIGH_SCORE_ADDR, high_score);
}

void
SnakeGameImpl::drawGame() {
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_5x7_mf);

    // Read strings from PROGMEM and draw
    strcpy_P(buffer, str_score);
    u8g2.drawStr(0, 6, buffer);
    u8g2.setCursor(30, 6);
    u8g2.print(score);

    strcpy_P(buffer, str_best);
    u8g2.drawStr(70, 6, buffer);
    u8g2.setCursor(95, 6);
    u8g2.print(high_score);

    for (int i = 0; i < snake_length; i++) {
        if (i == 0) {
            u8g2.drawBox(snake[i].x, snake[i].y, GRID_SIZE, GRID_SIZE);
        } else {
            u8g2.drawFrame(snake[i].x, snake[i].y, GRID_SIZE, GRID_SIZE);
        }
    }

    if (snake_length < SNAKE_MAX) {
        u8g2.drawBox(food.x + 1, food.y + 1, GRID_SIZE - 2, GRID_SIZE - 2);
    }

    u8g2.drawFrame(0, 8, SCREEN_WIDTH, SCREEN_HEIGHT);

    u8g2.sendBuffer();
}

void
SnakeGameImpl::drawGameOver() {
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_7x13_mf);

    if (snake_length >= SNAKE_MAX) {
        strcpy_P(buffer, str_you_win);
        u8g2.drawStr(30, 20, buffer);

        u8g2.setFont(u8g2_font_5x7_mf);
        strcpy_P(buffer, str_max_length);
        u8g2.drawStr(25, 30, buffer);
    } else {
        strcpy_P(buffer, str_game_over);
        u8g2.drawStr(25, 20, buffer);
    }

    u8g2.setFont(u8g2_font_5x7_mf);

    if (new_high_score) {
        strcpy_P(buffer, str_new_high);
        u8g2.drawStr(15, 42, buffer);
    }

    strcpy_P(buffer, str_score);
    u8g2.drawStr(36, 52, buffer);
    u8g2.setCursor(71, 52);
    u8g2.print(score);

    strcpy_P(buffer, str_press_btn);
    u8g2.drawStr(20 , 62 , buffer);

    u8g2.sendBuffer();
}

void
SnakeGameImpl::resetGame() {
    snake_length = 3;

    snake[0].x = GRID_SIZE * 10;
    snake[0].y = 8 + GRID_SIZE * 4;

    snake[1].x = GRID_SIZE * 9;
    snake[1].y = 8 + GRID_SIZE * 4;

    snake[2].x = GRID_SIZE * 8;
    snake[2].y = 8 + GRID_SIZE * 4;

    current_direction = 0;
    next_direction = 0;
    is_direction_changed = false;
    score = 0;
    game_over = false;
    food_eaten = false;
    game_speed = 500;
    new_high_score = false;

    generateFood();
}

////////////////////////////////////////////////////////////////////////////////
SnakeGame::SnakeGame(Oled& oled, DeviceInputs& userInputs)
{
    static_assert(sizeof(impl_) >= sizeof(SnakeGameImpl), "fix SnameGame::impl_ size");
    new (impl_) SnakeGameImpl(oled, userInputs);
}

void
SnakeGame::init(BlindCall nextMenuCb, BlindCall prevMenuCb)
{
    impl()->init(nextMenuCb, prevMenuCb);
}

bool
SnakeGame::setup()
{
    return impl()->setup();
}

void
SnakeGame::loop_step()
{
    impl()->loop_step();
}

void
SnakeGame::activate()
{
    impl()->activate();
}

void
SnakeGame::deactivate()
{
    impl()->deactivate();
}

void
SnakeGame::draw()
{
    impl()->drawGame();
}

SnakeGameImpl*
SnakeGame::impl()
{
    return reinterpret_cast<SnakeGameImpl*>(impl_);
}

const SnakeGameImpl*
SnakeGame::impl() const
{
    return reinterpret_cast<const SnakeGameImpl*>(impl_);
}
