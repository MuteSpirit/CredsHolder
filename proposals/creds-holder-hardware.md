# Hardware

No SMD components. The device MUST be able to make at home with favorite soldering station using components from Ali Express

Add 3-colored LED to have ability show current mode - locked, authentication, unlocked

## Controller

Arduino Pro Micro

NRF52840 - with TrustZone - https://ali.click/aj86j1w: 
* 64MHz, 256kB RAM, 1MB flash
* ARM TrustZone CryptoCell-310 - https://www.micro-semiconductor.com/datasheet/c0-NRF52840-QIAA-R.pdf

STB32U5 - with TrustZone - https://ali.click/5i86j1h:
* 160MHz, 786kB RAM, 2MB flash
* Cortex-M33 TrustZone - https://www.st.com/resource/en/application_note/an5421-getting-started-with-stm32-mcus-and-armtrustzone-development-stmicroelectronics.pdf

In Ledger Crypto Wallet
* MCU: STM32WB35
* Secure Element: ST33K1M5C

## Peripherals

Gyroscope and accelerometer:
- GY-LSM6DS33 - 3 axis, interrupts are supported, libraries from Adafruit and Pololu
- GY-521 (MPU-6050) - easy to start, many tutorials and libraries
* BMI160 - ADC (analog digit converter) inside, enough accuracy and low power consumption

### Random generator

TBD

For Arduino - WDT signals, library Entropy 

### Keyboard emulation

https://github.com/mooltipass/minible#manual-credential-typing
```
Manual Credential Typing
Mooltipass devices can simulate key presses in order to type logins & passwords onto the computer they're connected to. As the "byte sent on the wire" to "actual character typed on the computer" conversion is handled by the computer itself, that meant the Mooltipass devices need to handle multiple keyboard layouts.
For the Mooltipass Mini BLE, we are parsing the CLDR to make sure we can type text on any device.
```

# Links

* ETL (C++ Embedded STL) https://www.etlcpp.com/
* https://github.com/thirdpin/pastilda/
* https://github.com/trezor/trezor-firmware/tree/main
* https://github.com/mooltipass
* https://github.com/LedgerHQ/
* [nrf52 immutable bootloader](https://nrfconnectdocs.nordicsemi.com/ncs/2.0.1/nrf/samples/bootloader/README.html)
* [nrf52 secure bootloader chain](https://nrfconnectdocs.nordicsemi.com/ncs/2.0.1/nrf/ug_bootloader.html#immutable-bootloader)
  * `The immutable bootloader is flash-locked and cannot be modified or deleted without erasing the entire device.`
