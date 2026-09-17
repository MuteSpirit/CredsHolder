// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#include "device.hpp"
#include <new>
#include <math.h>

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// To be able handle MPU6050 events in time and avoid chip FIFO overloading
// it's needed to decrease speed of evetns generation to acceptable level
// #define MPU6050_DMP_FIFO_RATE_DIVISOR 0x31 // work enough stable
#define MPU6050_DMP_FIFO_RATE_DIVISOR 0x10 // ~ 60 Hz

#include "MPU6050/I2Cdev.h"
#include "MPU6050/MPU6050_6Axis_MotionApps20.h"
// TODO: try newer firmware from
// #include "MPU6050/MPU6050_6Axis_MotionApps612.h"
#include "MPU6050/MPU6050.h"

////////////////////////////////////////////////////////////////////////////////
enum class Movement : uint8_t
{
    clockwise,
    ccw, // counterclockwise
    none
};

struct SplashDetectCtx
{
    float ema {0.0f}; /// Exponential moving average

    /// like for keyboard we must detect when splash dissapear
    /// to react "not on button push but pull"
    uint8_t splashStarted {false};

    Movement mv {Movement::none};
};

struct CredsHolderInputsData
{
    CredsHolderInputsData();

    BlindCall hooks_[static_cast<uint8_t>(DeviceInputs::UserAction::size)];

    MPU6050 mpu_;
    uint16_t packetSize_;    // expected DMP packet size (default is 42 bytes)
    uint8_t fifoBuffer_[64]; // FIFO storage buffer
                             //
    SplashDetectCtx pitchDetectCtx_;
    SplashDetectCtx rollDetectCtx_;
};

static volatile bool mpuInterrupt {false}; // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

CredsHolderInputsData::CredsHolderInputsData()
{
    for (size_t i = 0; i < static_cast<uint8_t>(DeviceInputs::UserAction::size); ++i) {
        hooks_[i] = BlindCall::stub();
    }
}

////////////////////////////////////////////////////////////////////////////////
CredsHolderInputs::CredsHolderInputs()
{
    new(data_) CredsHolderInputsData();
}

CredsHolderInputsData* CredsHolderInputs::data()
{
    return reinterpret_cast<CredsHolderInputsData*>(data_);
}

void
CredsHolderInputs::set(UserAction act, BlindCall cb)
{
    data()->hooks_[static_cast<uint8_t>(act)] = cb;
}

void
CredsHolderInputs::unset(UserAction act)
{
    data()->hooks_[static_cast<uint8_t>(act)] = BlindCall::stub();
}

bool
CredsHolderInputs::setup(void)
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    // Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
#endif

    MPU6050* mpu = &data()->mpu_;

    mpu->initialize();

    uint8_t devStatus = mpu->dmpInitialize();
    if (devStatus != 0) {
        return false;
    }

    // TODO: make initial calibration, store init values in internal memory and reuse when they are present
    // for my concrete MPU6050 module next offsets has been detected during calibration:
    mpu->setXAccelOffset(2508);
    mpu->setYAccelOffset(-2151);
    mpu->setZAccelOffset(992);

    mpu->setXGyroOffset(-147);
    mpu->setYGyroOffset(25);
    mpu->setZGyroOffset(68); 

    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu->CalibrateAccel(6);
    mpu->CalibrateGyro(6);

    // turn on the DMP, now that it's ready
    mpu->setDMPEnabled(true);

    // enable Arduino interrupt detection
    attachInterrupt(digitalPinToInterrupt(MPU6050_CS_PIN), dmpDataReady, RISING);

    // TODO: add interrupts handling
    // uint8_t mpuIntStatus = mpu->getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it

    // get expected DMP packet size for later comparison
    data()->packetSize_ = mpu->dmpGetFIFOPacketSize();

    return true;
}

static bool isTiltHappen(SplashDetectCtx &ctx, const float p);

void
CredsHolderInputs::loop_step(void)
{
    if (!data()->mpu_.dmpGetCurrentFIFOPacket(data()->fifoBuffer_)) { // Get the Latest packet }
        return;
    }

    VectorFloat gravity;    // [x, y, z]            gravity vector
    Quaternion q;           // [w, x, y, z]         quaternion container
    float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

    data()->mpu_.dmpGetQuaternion(&q, data()->fifoBuffer_);
    data()->mpu_.dmpGetGravity(&gravity, &q);
    data()->mpu_.dmpGetYawPitchRoll(ypr, &q, &gravity);

    if (isTiltHappen(data()->pitchDetectCtx_, ypr[1] * 180 / M_PI)) {
        if (data()->pitchDetectCtx_.mv == Movement::clockwise) {
            data()->hooks_[static_cast<uint8_t>(UserAction::up)]();
        } else {
            data()->hooks_[static_cast<uint8_t>(UserAction::down)]();
        }
    }
    if (isTiltHappen(data()->rollDetectCtx_, ypr[2] * 180 / M_PI)) {
        if (data()->rollDetectCtx_.mv == Movement::clockwise) {
            data()->hooks_[static_cast<uint8_t>(UserAction::left)]();
        } else {
            data()->hooks_[static_cast<uint8_t>(UserAction::right)]();
        }
    }
//
}

static
bool
isTiltHappen(SplashDetectCtx &ctx, const float p)
{
  constexpr float threshold = 10; // usually tilt angle is about 15 degrees
  constexpr float alpha = 0.15;

  ctx.ema = alpha * p + (1.0 - alpha) * ctx.ema;

  float delta = p - ctx.ema;

  if (abs(delta) >= threshold) {
    ++ctx.splashStarted;
    // FIXME: we may quickly tilt device up and down and "mv" will
    // be updated to the latest tilt accordingly
    ctx.mv = (delta >= 0) ? Movement::clockwise : Movement::ccw;

    if (ctx.splashStarted >= 3) {
      // MPU-6050 DMP is configured to measure 1 time per second
      // so if we see that splash is happening 3 second in a raw
      // then it's just a new position of device
      // TODO: decide how detect "long tilt"
      ctx.splashStarted = 0;
      ctx.mv = Movement::none;
    }
    return false;
  } else {
    if (ctx.splashStarted > 0) {
      // we return device back to original position
      // so tilt finished and device may start a reaction
      ctx.splashStarted = 0;
      return true;
    } else {
      ctx.mv = Movement::none;
      return false;
    }
  }
}
