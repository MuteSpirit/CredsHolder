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
#if defined(EPOXY_DUINO)
#include "knock_sensor.hpp"

#include <Embedded_Template_Library.h>
#include <etl/array.h>

// Must be included as the last one to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

class KnockSensorTest : public aunit::TestOnce
{
protected:
    virtual void setup() override
    {
        TestOnce::setup();
        kd.begin(0.0f);
        assertNear(0.0f, kd.baseline(), 0.0f);

        assertEqual(50, KnockSensor::KNOCK_THRESHOLD);
        assertEqual(600, KnockSensor::MAX_KNOCK_GAP_MS);
        assertEqual(100, KnockSensor::MIN_KNOCK_GAP_MS);
    }

protected:
    KnockSensor kd;

    constexpr static const KnockSensor::Knock NO = KnockSensor::Knock::no;
    constexpr static const KnockSensor::Knock ONE = KnockSensor::Knock::single;
    constexpr static const KnockSensor::Knock TWO = KnockSensor::Knock::twin;

    constexpr static const uint16_t knockPeak = KnockSensor::KNOCK_THRESHOLD;
    constexpr static const uint16_t maxGap = KnockSensor::MAX_KNOCK_GAP_MS;
    constexpr static const uint16_t minGap = KnockSensor::MIN_KNOCK_GAP_MS;
};


testF(KnockSensorTest, zero_baseline_no_knock)
{
    etl::array<uint32_t, 10> analogPinValues {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    etl::array<uint32_t, 10> timeMs          {{ 0, 50, 100, 150, 200, 250, 300, 350, 400, 450}};

    for (size_t i = 0; i < analogPinValues.size(); ++i) {
        assertEqual((uint8_t)KnockSensor::Knock::no, (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
    }

    assertNear(0.0f, kd.baseline(), 0.0f);
}

testF(KnockSensorTest, baseline_more_zero_no_knock)
{
    etl::array<uint32_t, 10> analogPinValues {{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    etl::array<uint32_t, 10> timeMs          {{ 0, 50, 100, 150, 200, 250, 300, 350, 400, 450}};

    for (size_t i = 0; i < analogPinValues.size(); ++i) {
        assertEqual((uint8_t)KnockSensor::Knock::no, (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
    }

    assertNear(7.5f, kd.baseline(), 0.4f);
}

testF(KnockSensorTest, zero_baseline_knock)
{
    etl::array<uint32_t, 3> analogPinValues {{ 0, knockPeak + 1,          0}};
    etl::array<uint32_t, 3> timeMs          {{ 0,        maxGap, 2 * maxGap}};

    size_t i = 0;
    for (; i < analogPinValues.size() - 1; ++i) {
        assertEqual((uint8_t)KnockSensor::Knock::no, (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
    }
    assertNear(0.0f, kd.baseline(), 0.0f);
    assertEqual((uint8_t)KnockSensor::Knock::single, (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
}

testF(KnockSensorTest, non_zero_baseline_knock)
{
    etl::array<uint32_t, 5> analogPinValues {{    5,    7,    8, knockPeak + 6, knockPeak - 6}};
    etl::array<uint32_t, 5> timeMs          {{ 1000, 1050, 1100,          1150, 1150+maxGap}};

    size_t i = 0;
    for (; i < analogPinValues.size() - 1; ++i) {
        assertEqual((uint8_t)KnockSensor::Knock::no, (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
    }
    assertNear(5.5f, kd.baseline(), 0.5f);
    assertEqual((uint8_t)KnockSensor::Knock::single, (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
}

// Let's guess that schema with 1MOhm resistor between analog pin and ground will always keep baseline near zero
// testF(KnockSensorTest, jump_to_bigger_baseline_without_knock_detection)
// {
//     KnockSensor kd;
//
//     kd.begin(0.0f);
//     assertEqual(10, KnockSensor::KNOCK_THRESHOLD);
//     assertNear(0.0f, kd.baseline(), 0.0f);
//     {
//         etl::array<uint32_t, 10> analogPinValues {{ 5, 7, 8, 6, 10, 25, 27, 30, 31, 35}};
//         etl::array<uint32_t, 10> timeMs          {{ 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450}};
//
//         size_t i = 0;
//         for (; i < analogPinValues.size() - 1; ++i) {
//             assertEqual((uint8_t)KnockSensor::Knock::no, (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
//         }
//     }
// }

testF(KnockSensorTest, debounce)
{
    // need gap between 0 and 1st point >= MIN_KNOCK_GAP_MS
    etl::array<uint32_t, 4> analogPinValues {{    0, knockPeak + 1, knockPeak + 5,           0}};
    etl::array<uint32_t, 4> timeMs          {{ 1000,          1050,          1100, 1150+maxGap}};

    int detected = 0;

    for (size_t i = 0; i < analogPinValues.size(); ++i) {
        if (KnockSensor::Knock::single == kd.update(analogPinValues[i], timeMs[i])) {
            ++detected;
        }
    }

    assertEqual(1, detected);
}

testF(KnockSensorTest, double_knock)
{
    // two knocks with 100 ms period between
    etl::array<uint32_t, 7> analogPinValues {{ knockPeak+1, knockPeak+1, 0, 0, knockPeak+2, knockPeak+2, 0}};
    etl::array<uint32_t, 7> timeMs          {{ 1000, 1050, 1100, 1150, 1200, 1250,                       1250+maxGap}};

    int singleKnocksDetected = 0;
    int doubleKnocksDetected = 0;

    for (size_t i = 0; i < analogPinValues.size(); ++i) {
        switch (kd.update(analogPinValues[i], timeMs[i])) {
            case KnockSensor::Knock::single:
                ++singleKnocksDetected;
                break;
            case KnockSensor::Knock::twin:
                ++doubleKnocksDetected;
                break;
            default:
                break;
        }
    }

    assertEqual(0, singleKnocksDetected);
    assertEqual(1, doubleKnocksDetected);
}

testF(KnockSensorTest, two_single_knocks)
{
    //                                       delay between knocks is more then MAX_KNOCK_GAP_MS
    etl::array<uint32_t, 7> analogPinValues {{ knockPeak+1, knockPeak+1,    0,    0,        knockPeak+2, knockPeak+2,    0}};
    etl::array<uint32_t, 7> timeMs          {{        1000,        1050, 1100, 1150+maxGap, 1200+maxGap, 1250+maxGap, 1300+2*maxGap}};

    int singleKnocksDetected = 0;
    int doubleKnocksDetected = 0;

    for (size_t i = 0; i < analogPinValues.size(); ++i) {
        switch (kd.update(analogPinValues[i], timeMs[i])) {
            case KnockSensor::Knock::single:
                ++singleKnocksDetected;
                break;
            case KnockSensor::Knock::twin:
                ++doubleKnocksDetected;
                break;
            default:
                break;
        }
    }

    assertEqual(2, singleKnocksDetected);
    assertEqual(0, doubleKnocksDetected);
}

testF(KnockSensorTest, knock_after_double_knocking)
{
    constexpr const size_t cSamples = 10;

    etl::array<uint32_t, cSamples> analogPinValues {{ knockPeak+1, knockPeak+1, knockPeak+1,  knockPeak+2, knockPeak+2,           0,         knockPeak+3,    knockPeak+3,   knockPeak+3,             0 }};
    etl::array<uint32_t, cSamples> timeMs          {{  1000, 1050, 1100,                      1100+minGap, 1150+minGap, 1200+minGap,  1250+minGap+maxGap, 1300+minGap+maxGap, 1350+minGap+maxGap, 1400+minGap+2*maxGap}};
    etl::array<KnockSensor::Knock, cSamples> res   {{  NO, NO, NO,                   NO,  NO,    NO,           TWO,  NO,  NO,            ONE}};

    for (size_t i = 0; i < cSamples; ++i) {
        assertEqual((uint8_t)res[i], (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
    }
}

testF(KnockSensorTest, defence_on_broken_timeline)
{
    constexpr const size_t cSamples = 3;

    etl::array<uint32_t, cSamples> analogPinValues {{     0,   20,    0 }};
    etl::array<uint32_t, cSamples> timeMs          {{  1000, 2000, 1500 }};
    etl::array<KnockSensor::Knock, cSamples> res   {{    NO,   NO,   NO }};

    for (size_t i = 0; i < cSamples; ++i) {
        assertEqual((uint8_t)res[i], (uint8_t)kd.update(analogPinValues[i], timeMs[i]));
    }
    
}

#endif // defined(EPOXY_DUINO)
