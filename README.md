# CredsHolder

Hardware credentials manager.

Secure replacement of stickers on monitor with login and passwords.

CredsHolder makes it easier to use good complex and unique passwords for your accounts on different web sites and other applications. 

<img src="./docs/slide-monitor-with-stickers.png" alt="Presentation" width=768 height=1024>

# Project News

## 03 Jun 2026

* All original code of PasswordPump has been removed from the project. Code base has been developed from scratch.
* AES128 has been replaced with AES256.
* Code related to authentication and encrypted storage has been covered by unit tests (AUnit has been used).
* Unfortunatelly **sketch is too big for Arduino Pro Micro** and need to decide how continue the project

## 06 Jun 2026

* Porting code to nRF52840 Pro Micro board is starting
* Changing project license is planned

## 07 Aug 2026

* Project has been renamed and started from scratch
  * Reason 1: license has been changed to MIT. I hope that will help for distribution.
  * Reason 2: device redesign is needed to be long supported.
  * Reason 3: to be not fork of PasswordPump will allow to configure Git LFS and store docs pictures originals.

## This project is fork of ...

... https://github.com/seawarrior181/PasswordPump

I'd like to thank Dan Murphy for PasswordPump. It's cool idea and device.
I'm appreciate how many features has been implemented in orignal sketch.

### Forking Reasons

PasswordPump usability is too complex by my opinion.
I'm sure that it's possible to make device more simple and usable.

PasswordPumpII project is exist but it's based on board which price is in 5-7 times higher then for Arduino Pro Micro. Such cool device as hardware credential manager should be as cheaper as possible to be more accecible.
