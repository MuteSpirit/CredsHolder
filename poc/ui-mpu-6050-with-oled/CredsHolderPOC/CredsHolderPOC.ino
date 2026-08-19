//
// Code is based on examples in https://github.com/jrowberg/i2cdevlib
// isTiltHappen is writted additionally

#include <Adafruit_SSD1306.h>

// #define MPU6050_DMP_FIFO_RATE_DIVISOR 0x31 // 20 Hz - work enough stable
#define MPU6050_DMP_FIFO_RATE_DIVISOR 0x13 // 40 Hz - also stable

#include "MPU6050/I2Cdev.cpp"
#include "MPU6050/MPU6050_6Axis_MotionApps20.cpp"
// #include "MPU6050/MPU6050_6Axis_MotionApps612.cpp"
#include "MPU6050/MPU6050.cpp"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

/* =========================================================================
   NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the Arduino's
   external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
   digital I/O pin 2.
   ========================================================================= */

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 gy;         // [x, y, z]            gyro sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


uint8_t nLines = 0; // increment on each display.println

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

SplashDetectCtx pitchDetectCtx;
SplashDetectCtx rollDetectCtx;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

// ================================================================
// ===                      TILT DETECTION                      ===
// ================================================================


bool isTiltHappen(const float *p, const size_t sz, Movement& mv);


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize serial communication
  // (115200 chosen because it is required for Teapot Demo output, but it's
  // really up to you depending on your project)
  Serial.begin(38400);
  // while (!Serial); // wait for Leonardo enumeration, others continue immediately

  // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3V or Arduino
  // Pro Mini running at 3.3V, cannot handle this baud rate reliably due to
  // the baud timing being too misaligned with processor ticks. You must use
  // 38400 or slower in these cases, or use some kind of external separate
  // crystal solution for the UART timer.

  // initialize device
  if (Serial) { Serial.println(F("Initializing I2C devices...")); }
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  if (Serial) { Serial.println(F("Testing device connections...")); }
  if (Serial) { Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed")); }

  // load and configure the DMP
  if (Serial) { Serial.println(F("Initializing DMP fifo rate is 60 Hz and gyro +/- 1000 deg/sec]...")); }
  devStatus = mpu.dmpInitialize();

  // for my concrete MPU6050 module next offsets has been detected during calibration:
  mpu.setXAccelOffset(2508);
  mpu.setYAccelOffset(-2151);
  mpu.setZAccelOffset(992);

  mpu.setXGyroOffset(-147);
  mpu.setYGyroOffset(25);
  mpu.setZGyroOffset(68); 

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    if (Serial) { Serial.println(); }
    mpu.PrintActiveOffsets();
    // turn on the DMP, now that it's ready
    if (Serial) { Serial.println(F("Enabling DMP...")); }
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    if (Serial) { Serial.print(F("Enabling interrupt detection (Arduino external interrupt ")); }
    if (Serial) { Serial.print(digitalPinToInterrupt(INTERRUPT_PIN)); }
    if (Serial) { Serial.println(F(")...")); }
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    if (Serial) { Serial.println(F("DMP ready! Waiting for first interrupt...")); }
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    if (Serial) { Serial.print(F("DMP Initialization failed (code ")); }
    if (Serial) { Serial.print(devStatus); }
    if (Serial) { Serial.println(F(")")); }
  }

  // configure LED for output
  pinMode(LED_PIN, OUTPUT);

  // OLED
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    if (Serial) { Serial.println(F("SSD1306 allocation failed")); }
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================



void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;
  // read a packet from FIFO
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet 

#ifdef OUTPUT_READABLE_YAWPITCHROLL
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

      if (nLines > 20) {
        nLines = 0;
        display.clearDisplay();
        display.setCursor(0, 0);
      }

      if (isTiltHappen(pitchDetectCtx, ypr[1] * 180 / M_PI)) {
        display.print(pitchDetectCtx.mv == Movement::clockwise ? " Up" : " Down"); ++nLines;
      }
      if (isTiltHappen(rollDetectCtx, ypr[2] * 180 / M_PI)) {
        display.print(rollDetectCtx.mv == Movement::clockwise ? " <=" : " =>"); ++nLines;
      }

      display.display();
#endif
    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
  }
}

bool isTiltHappen(SplashDetectCtx &ctx, const float p)
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

////////////////////////////////////////////////////////////////////////////////
// Calibration
////////////////////////////////////////////////////////////////////////////////
// // class default I2C address is 0x68
// // specific I2C addresses may be passed as a parameter here
// // AD0 low = 0x68 (default for InvenSense evaluation board)
// // AD0 high = 0x69
// MPU6050 accelgyro;
// //MPU6050 accelgyro(0x69); // <-- use for AD0 high
//
//
// const char LBRACKET = '[';
// const char RBRACKET = ']';
// const char COMMA    = ',';
// const char BLANK    = ' ';
// const char PERIOD   = '.';
//
// const int iAx = 0;
// const int iAy = 1;
// const int iAz = 2;
// const int iGx = 3;
// const int iGy = 4;
// const int iGz = 5;
//
// const int usDelay = 3150;   // empirical, to hold sampling to 200 Hz
// const int NFast =  1000;    // the bigger, the better (but slower)
// const int NSlow = 10000;    // ..
// const int LinesBetweenHeaders = 5;
//       int LowValue[6];
//       int HighValue[6];
//       int Smoothed[6];
//       int LowOffset[6];
//       int HighOffset[6];
//       int Target[6];
//       int LinesOut;
//       int N;
//
// void ForceHeader()
//   { LinesOut = 99; }
//
// void GetSmoothed()
//   { int16_t RawValue[6];
//     int i;
//     long Sums[6];
//     for (i = iAx; i <= iGz; i++)
//       { Sums[i] = 0; }
// //    unsigned long Start = micros();
//
//     for (i = 1; i <= N; i++)
//       { // get sums
//         accelgyro.getMotion6(&RawValue[iAx], &RawValue[iAy], &RawValue[iAz], 
//                              &RawValue[iGx], &RawValue[iGy], &RawValue[iGz]);
//         if ((i % 500) == 0)
//           Serial.print(PERIOD);
//         delayMicroseconds(usDelay);
//         for (int j = iAx; j <= iGz; j++)
//           Sums[j] = Sums[j] + RawValue[j];
//       } // get sums
// //    unsigned long usForN = micros() - Start;
// //    Serial.print(" reading at ");
// //    Serial.print(1000000/((usForN+N/2)/N));
// //    Serial.println(" Hz");
//     for (i = iAx; i <= iGz; i++)
//       { Smoothed[i] = (Sums[i] + N/2) / N ; }
//   } // GetSmoothed
//
// void Initialize()
//   {
//     // join I2C bus (I2Cdev library doesn't do this automatically)
//     #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
//         Wire.begin();
//     #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
//         Fastwire::setup(400, true);
//     #endif
//
//     Serial.begin(9600);
//
//     // initialize device
//     Serial.println("Initializing I2C devices...");
//     accelgyro.initialize();
//
//     // verify connection
//     Serial.println("Testing device connections...");
//     Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
//     Serial.println("PID tuning Each Dot = 100 readings");
//   /*A tidbit on how PID (PI actually) tuning works. 
//     When we change the offset in the MPU6050 we can get instant results. This allows us to use Proportional and 
//     integral of the PID to discover the ideal offsets. Integral is the key to discovering these offsets, Integral 
//     uses the error from set-point (set-point is zero), it takes a fraction of this error (error * ki) and adds it 
//     to the integral value. Each reading narrows the error down to the desired offset. The greater the error from 
//     set-point, the more we adjust the integral value. The proportional does its part by hiding the noise from the 
//     integral math. The Derivative is not used because of the noise and because the sensor is stationary. With the 
//     noise removed the integral value lands on a solid offset after just 600 readings. At the end of each set of 100 
//     readings, the integral value is used for the actual offsets and the last proportional reading is ignored due to 
//     the fact it reacts to any noise.
//   */
//         accelgyro.CalibrateAccel(6);
//         accelgyro.CalibrateGyro(6);
//         Serial.println("\nat 600 Readings");
//         accelgyro.PrintActiveOffsets();
//         Serial.println();
//         accelgyro.CalibrateAccel(1);
//         accelgyro.CalibrateGyro(1);
//         Serial.println("700 Total Readings");
//         accelgyro.PrintActiveOffsets();
//         Serial.println();
//         accelgyro.CalibrateAccel(1);
//         accelgyro.CalibrateGyro(1);
//         Serial.println("800 Total Readings");
//         accelgyro.PrintActiveOffsets();
//         Serial.println();
//         accelgyro.CalibrateAccel(1);
//         accelgyro.CalibrateGyro(1);
//         Serial.println("900 Total Readings");
//         accelgyro.PrintActiveOffsets();
//         Serial.println();    
//         accelgyro.CalibrateAccel(1);
//         accelgyro.CalibrateGyro(1);
//         Serial.println("1000 Total Readings");
//         accelgyro.PrintActiveOffsets();
//      Serial.println("\n\n Any of the above offsets will work nice \n\n Lets proof the PID tuning using another method:"); 
//   } // Initialize
//
// void SetOffsets(int TheOffsets[6])
//   { accelgyro.setXAccelOffset(TheOffsets [iAx]);
//     accelgyro.setYAccelOffset(TheOffsets [iAy]);
//     accelgyro.setZAccelOffset(TheOffsets [iAz]);
//     accelgyro.setXGyroOffset (TheOffsets [iGx]);
//     accelgyro.setYGyroOffset (TheOffsets [iGy]);
//     accelgyro.setZGyroOffset (TheOffsets [iGz]);
//   } // SetOffsets
//
// void ShowProgress()
//   { if (LinesOut >= LinesBetweenHeaders)
//       { // show header
//         Serial.println("\tXAccel\t\t\tYAccel\t\t\t\tZAccel\t\t\tXGyro\t\t\tYGyro\t\t\tZGyro");
//         LinesOut = 0;
//       } // show header
//     Serial.print(BLANK);
//     for (int i = iAx; i <= iGz; i++)
//       { Serial.print(LBRACKET);
//         Serial.print(LowOffset[i]),
//         Serial.print(COMMA);
//         Serial.print(HighOffset[i]);
//         Serial.print("] --> [");
//         Serial.print(LowValue[i]);
//         Serial.print(COMMA);
//         Serial.print(HighValue[i]);
//         if (i == iGz)
//           { Serial.println(RBRACKET); }
//         else
//           { Serial.print("]\t"); }
//       }
//     LinesOut++;
//   } // ShowProgress
//
// void PullBracketsIn()
//   { boolean AllBracketsNarrow;
//     boolean StillWorking;
//     int NewOffset[6];
//
//     Serial.println("\nclosing in:");
//     AllBracketsNarrow = false;
//     ForceHeader();
//     StillWorking = true;
//     while (StillWorking) 
//       { StillWorking = false;
//         if (AllBracketsNarrow && (N == NFast))
//           { SetAveraging(NSlow); }
//         else
//           { AllBracketsNarrow = true; }// tentative
//         for (int i = iAx; i <= iGz; i++)
//           { if (HighOffset[i] <= (LowOffset[i]+1))
//               { NewOffset[i] = LowOffset[i]; }
//             else
//               { // binary search
//                 StillWorking = true;
//                 NewOffset[i] = (LowOffset[i] + HighOffset[i]) / 2;
//                 if (HighOffset[i] > (LowOffset[i] + 10))
//                   { AllBracketsNarrow = false; }
//               } // binary search
//           }
//         SetOffsets(NewOffset);
//         GetSmoothed();
//         for (int i = iAx; i <= iGz; i++)
//           { // closing in
//             if (Smoothed[i] > Target[i])
//               { // use lower half
//                 HighOffset[i] = NewOffset[i];
//                 HighValue[i] = Smoothed[i];
//               } // use lower half
//             else
//               { // use upper half
//                 LowOffset[i] = NewOffset[i];
//                 LowValue[i] = Smoothed[i];
//               } // use upper half
//           } // closing in
//         ShowProgress();
//       } // still working
//
//   } // PullBracketsIn
//
// void PullBracketsOut()
//   { boolean Done = false;
//     int NextLowOffset[6];
//     int NextHighOffset[6];
//
//     Serial.println("expanding:");
//     ForceHeader();
//
//     while (!Done)
//       { Done = true;
//         SetOffsets(LowOffset);
//         GetSmoothed();
//         for (int i = iAx; i <= iGz; i++)
//           { // got low values
//             LowValue[i] = Smoothed[i];
//             if (LowValue[i] >= Target[i])
//               { Done = false;
//                 NextLowOffset[i] = LowOffset[i] - 1000;
//               }
//             else
//               { NextLowOffset[i] = LowOffset[i]; }
//           } // got low values
//
//         SetOffsets(HighOffset);
//         GetSmoothed();
//         for (int i = iAx; i <= iGz; i++)
//           { // got high values
//             HighValue[i] = Smoothed[i];
//             if (HighValue[i] <= Target[i])
//               { Done = false;
//                 NextHighOffset[i] = HighOffset[i] + 1000;
//               }
//             else
//               { NextHighOffset[i] = HighOffset[i]; }
//           } // got high values
//         ShowProgress();
//         for (int i = iAx; i <= iGz; i++)
//           { LowOffset[i] = NextLowOffset[i];   // had to wait until ShowProgress done
//             HighOffset[i] = NextHighOffset[i]; // ..
//           }
//      } // keep going
//   } // PullBracketsOut
//
// void SetAveraging(int NewN)
//   { N = NewN;
//     Serial.print("averaging ");
//     Serial.print(N);
//     Serial.println(" readings each time");
//    } // SetAveraging
//
// void setup()
//   { Initialize();
//     for (int i = iAx; i <= iGz; i++)
//       { // set targets and initial guesses
//         Target[i] = 0; // must fix for ZAccel 
//         HighOffset[i] = 0;
//         LowOffset[i] = 0;
//       } // set targets and initial guesses
//     Target[iAz] = 16384;
//     SetAveraging(NFast);
//
//     PullBracketsOut();
//     PullBracketsIn();
//
//     Serial.println("-------------- done --------------");
//   } // setup
//
// void loop()
//   {
//   } // loop
////////////////////////////////////////////////////////////////////////////////
// MPU6050 mpu;
//
// volatile bool mpuFlag = false;  // флаг прерывания готовности
// uint8_t fifoBuffer[45];         // буфер
//
// void setup() {
//   Serial.begin(115200);
//   Wire.begin();
//   //Wire.setClock(1000000UL);     // разгоняем шину на максимум
//
//   // инициализация DMP и прерывания
//   mpu.initialize();
//   mpu.dmpInitialize();
//   mpu.setDMPEnabled(true);
//   attachInterrupt(0, dmpReady, RISING);
// }
//
// // прерывание готовности. Поднимаем флаг
// void dmpReady() {
//   mpuFlag = true;
// }
//
// void loop() {
//   // по флагу прерывания и готовности DMP
//   if (mpuFlag && mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
//     // переменные для расчёта (ypr можно вынести в глобал)
//     Quaternion q;
//     VectorFloat gravity;
//     float ypr[3];
//
//     // расчёты
//     mpu.dmpGetQuaternion(&q, fifoBuffer);
//     mpu.dmpGetGravity(&gravity, &q);
//     mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
//     mpuFlag = false;
//
//     // выводим результат в радианах (-3.14, 3.14)
//     Serial.print(ypr[0]); // вокруг оси Z
//     Serial.print(',');
//     Serial.print(ypr[1]); // вокруг оси Y
//     Serial.print(',');
//     Serial.print(ypr[2]); // вокруг оси X
//     Serial.println();
//     // для градусов можно использовать degrees()
//   }
// }
////////////////////////////////////////////////////////////////////////////////
// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>
//
// Adafruit_MPU6050 mpu;
//
// void setup(void) {
//     Serial.begin(115200);
//
//     // Try to initialize!
//     if (!mpu.begin()) {
//         Serial.println("Failed to find MPU6050 chip");
//         while (1) {
//           delay(100);
//         }
//     }
//
//     // set accelerometer range to +-8G
//     mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//
//     // set gyro range to +- 500 deg/s
//     mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//
//     // set filter bandwidth to 21 Hz
//     mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
//
//     delay(100);
// }
//
// void loop() {
//     /* Get new sensor events with the readings */
//     sensors_event_t a, g, temp;
//     mpu.getEvent(&a, &g, &temp);
//
//     mpu.resetFIFO();
//
//     /* Print out the values */
//     Serial.print(a.acceleration.x);
//     Serial.print(",");
//     Serial.print(a.acceleration.y);
//     Serial.print(",");
//     Serial.print(a.acceleration.z);
//     Serial.print(", ");
//     Serial.print(g.gyro.x);
//     Serial.print(",");
//     Serial.print(g.gyro.y);
//     Serial.print(",");
//     Serial.print(g.gyro.z);
//     Serial.println("");
//
//     delay(50);
// }
//

// #include "MPU6050/MPU6050.cpp"
// #include "MPU6050/I2Cdev.cpp"
//
// MPU6050 mpu;
// int16_t ax, ay, az;
// int16_t gx, gy, gz;
//
// void setup() {
//   Wire.begin();
//   Serial.begin(9600);
//
//   mpu.initialize();
//   mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);
//   mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
//
//   if (mpu.testConnection()) {
//
//     Serial.println("MPU6050 OK");
//     mpu.CalibrateAccel(2);
//     mpu.CalibrateGyro(2);
//   } else {
//     Serial.println("MPU6050 FAIL");
//   }
//
//   delay(1000);
// }
//
// void loop() {
//   mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
//   Serial.print(ax); Serial.print('\t');
//   Serial.print(ay); Serial.print('\t');
//   Serial.print(az); Serial.print('\t');
//   Serial.print(gx); Serial.print('\t');
//   Serial.print(gy); Serial.print('\t');
//   Serial.println(gz);
//   delay(1000);
// }
