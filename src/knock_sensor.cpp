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
#include "knock_sensor.hpp"

#include "creds_holder.hpp"
#include <Arduino.h>

static const float EMA_ALPHA = 0.4f; /// Exponential moving average ratio

////////////////////////////////////////////////////////////////////////////////
void
KnockSensor::begin(const float baselineSeed)
{
    baseline_ = baselineSeed;
}

KnockSensor::Knock
KnockSensor::update(const uint32_t rawValue, const unsigned long nowMs)
{
    KnockSensor::Knock detected = Knock::no;

    if (nowMs < lastKnockMs_) {
        return detected;
    }
    lastMeasureMs_ = nowMs;

    const float diff = (float)rawValue - baseline_;

    const unsigned long sinceLastMeasureMs = nowMs - lastKnockMs_;
    const bool splashIsPresent = diff > KNOCK_THRESHOLD;
    const bool splashIsNew = splashIsPresent && (sinceLastMeasureMs >= MIN_KNOCK_GAP_MS);
    const bool noSplashEnoughLong = !splashIsPresent && (sinceLastMeasureMs >= MAX_KNOCK_GAP_MS);


    switch (state_) {
        case State::idle:
            if (splashIsPresent) {
                lastKnockMs_ = nowMs;

                if (splashIsNew) {
                    state_ = State::FirstKnock;
                } else { // 1st or 2nd splash is still continuing
                         // So ignore it
                    state_ = State::idle;
                }
            }
            break;
       case State::FirstKnock:
            if (splashIsPresent) {
                lastKnockMs_ = nowMs;

                if (splashIsNew) {
                    state_ = State::SecondKnock;
                }
            } else {
                if (noSplashEnoughLong) {
                    state_ = State::idle;
                    detected = Knock::single;
                }
            }
            break;
       case State::SecondKnock:
            if (splashIsPresent) {
                lastKnockMs_ = nowMs;

                if (splashIsNew) {
                    // 3rd knock is starting but it'll be marked as 1st knock of new knocking series
                    state_ = State::FirstKnock;
                    detected = Knock::twin;
                }
            } else {
                if (noSplashEnoughLong) {
                    state_ = State::idle;
                    detected = Knock::twin;
                }
            }
            break;
        default:
            if (Serial) { Serial.println(F("KnockSensor FSM unexpected state")); }
            break;
    }

    if (!splashIsPresent) {
        // Baseline update only if knock is absent because of assumption:
        //   1 MOhm resistor between analog pin and GND provides stable
        //   baseline near 0
        baseline_ = EMA_ALPHA * rawValue + (1.0 - EMA_ALPHA) * baseline_;
    }

    return detected;
}

float
KnockSensor::baseline() const
{
    return baseline_;
}

unsigned long
KnockSensor::lastKnockMs() const
{
    return lastKnockMs_;
}

unsigned long
KnockSensor::lastMeasureMs() const
{
    return lastMeasureMs_;
}
