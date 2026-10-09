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
#include <inttypes.h>

///  Baseline tracking, threshold check, debounce.
///
///  @details Class does NOT touch hardware - receives raw values from outside.
///  You must get baseline seed outside this class and call "begin" method to 
///  initiate KnockSensor
class KnockSensor {
public:
    static const uint16_t KNOCK_THRESHOLD = 50;  /// minimal knock deviation from baseline
    static const uint16_t MIN_KNOCK_GAP_MS = 100;
    static const uint16_t MAX_KNOCK_GAP_MS = 600;
    static const uint16_t MEASURE_INTERVAL = 50;

    enum class Knock : uint8_t
    {
        no,
        single,
        twin
    };

public:
    void begin(const float baselineSeed);

    /// Accept last knock sensor value to update internal FSM detecting knock. 
    /// Can detect single and double knocking.
    /// Call method only once for each new measurement.
    ///
    /// @details 
    /// Splash wave may continue several points (1, 2, 3, ???) and it's wrong
    /// to recognize peak value as new knock start if current splash continues
    /// longer then expected.
    /// MAX_KNOCK_GAP_MS is used to detect end of knocking series.
    /// If splash happen later then MIN_KNOCK_GAP_MS after previous one then it's
    /// treated as new splash and the same splash otherwise.
    ///
    /// @param[in] rawValue - [0, 1000] - Arduino compatible diapasone
    ///
    /// @return Knock::single or Knock::twin after end of knocking series, 
    /// otherwise - Knock::no.
    Knock update(const uint32_t rawValue, const unsigned long nowMs);

    unsigned long lastKnockMs() const;
    unsigned long lastMeasureMs() const;
    float baseline() const;

protected:
    enum class State : uint8_t
    {
        idle,
        FirstKnock,
        SecondKnock,
    };

protected:
    float         baseline_ {0.0f}; /// [0, 1000] - Arduino compatible diapasone
    unsigned long lastKnockMs_ {0};
    unsigned long lastMeasureMs_ {0};

    State state_ {State::idle}; /// finite machine state
};
