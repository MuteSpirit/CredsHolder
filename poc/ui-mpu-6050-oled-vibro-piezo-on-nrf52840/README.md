# POC No 5. Double knock detection

## Goal

Has one more type of User action type besides tilt.

## Proposals

Piezoelectric is used as sensor to detect tap.

There is position sensor MPU-6050 also but it's hard to recognize soft tap using it's measurements.
There was idea about shaking device but I could not understand what gyroscope/accelerometer curve it's needed to react on.
It's a point for future improvement.

## Plan

1. [x] Plug piezoelectric element as sensor
2. [x] Collect measurements from sensor on knocking it
3. [x] Make intermediate conclusions
4. [x] Enable vibration motor module several times and get known influence on piezoelectric sensor output.
5. [x] Make conclusions
6. [x] Add function for single and double knock detection

## Conclusions

* If 1 MOhm resistor is located between sensor contacts and there is conductor between analog output and sensor positive conductor then:
  * We see non-stable signal in diapason [0, 5] on analog pin
  * Signal baseline may jump to another level so some unknown reasons (temperature, energy accumulation by resistor, etc.)
* If 1 MOhm resistor is connected between analog pin and GND then base level is stably 0
* Knock can be detected by 10-15 peak values.
* Knock duration - about 50-100 ms. That fact defines frequency measurements requirement - each 50ms to catch enough big value for detection.
* Delay between two knocks is about 200 ms in my case.
* Single knock detection algorithm is similar to tilt start detection.
* Single knock detection is unusable because even taking device from the desk can be recognized as single knock and consequent action may be called
* Vibration influence on piezoelectric element and force it detect single knock
* It had to increase knock threshold up to 50 and time gap between knocks also because quick double tap has influence on piezoelectric element as single knock, maybe because it did not stop vibrate after first tap.
