# CredsHolder

Hardware credentials manager.

Secure replacement of stickers on monitor (and other similar stuff) with login and passwords.

CredsHolder makes it easier to use good complex and unique passwords for your accounts on different web sites and other applications. 

<img src="./docs/slide-monitor-with-stickers.png" alt="Slide 1" width=768 height=1024>

# See Also

* [CredsHolder Project Requirements Doc](proposals/creds-holder.md)

# Project News

Current stage - designing.

## 14 Aug 2026

* Finished [CredsHolder Storage High Level Design](proposals/creds-holder-storage.md)
  * Encrypted VeraCrypt volume on removable microSD card
  * PIN for CredsHolder unlock but custom complex password and non-default PIM for VeraCrypt volume encryption
  * FAT inside encrypted volume
  * Any suitable file format for application data, i.e. SQLite
  * See all details in design

## 13 Aug 2026

* Finished [CredsHolder Authentication High Level Design](proposals/creds-holder-auth.md)
  * PIN for CredsHolder authentication
  * No one button or encoder in Device
  * Enter PIN via Morse code
  * See all details in design

## 12 Aug 2026

* Finished [CredsHolder Project Requirements Doc](proposals/creds-holder.md)
* Low Level Design for Authentication is started

## 07 Aug 2026

* Close PasswordWand (clone of PasswordPump with modified code) project as deprecated
* Project has been renamed and started from scratch
  * Reason 1: license has been changed to MIT. I hope that will help for distribution.
  * Reason 2: device SW and HW redesign is needed. Original PasswordPump code base is very complex to be maintainable and extendable.
  * Reason 3: to be not a fork repo of PasswordPump will allow to configure Git LFS and store docs pictures originals in Git.
  * Reason 4: do at least OpenSource project design and development accordign all best practices and without rush.

## This project was originally fork of ...

... https://github.com/seawarrior181/PasswordPump

I'd like to thank Dan Murphy for PasswordPump. It's cool idea and device.
I'm appreciate how many features has been implemented in orignal sketch.
