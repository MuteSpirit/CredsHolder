# TODO

| :blue_book: | Reason(s) |
| --- | :--- |
| 1 | Tracking tasks is in Markdown to avoid dependency on GitHub embedded bugtracker |
| 2 | Task list is present in each repository backup |

## Abbrevs, Terms, etc.

"Device" - CredsHolder common title in proposals, manuals and other resources

## Sub-tasks

* [x] CONTRIBUTING.md: with development environment settings, docs style, folder structure
* [x] docs(CONTRIBUTING.md): description of HLD principals
* [x] docs: collect project requirements
* [x] docs(CONTRIBUTING.md): description of LLD principals
* [x] Propose Device UI
  * [x] POC1. Tilt detection on Arduino
  * [x] POC2. Tilt detection on NRF52840
* [x] Propose Device HW storage
* [x] Propose Device (encrypted) data storage
* [x] Propose Device file system
* [~] Propose menu navigation
  * without auth
  * menu Accounts
  * menu Settings (show/hide passwords)
  * typing login/password
  * no encryption, no storage on microSD, hard-coded accounts
* [ ] propose authentication
  * (blocked) vibration motor module is in delivery
* [ ] Implement calibration
* [ ] Implement tutorial
* [ ] Implement authentication by hard-coded pin
* [ ] Implement read file from microSD card with partition "storage" formatted with FAT16
* [ ] Implement formatting partition "storage" as VeraCrypt volume
* [ ] Implement reading from VeraCrypt volume
* [ ] Implement PIN on LUKS header in internal storage
* [ ] Implement set PIN by User
* [ ] Implement new way for accept/enter action - via tap/knock
  * (blocked) piezo electronic sensor is in delivery
* [ ] propose object storage
  * [~] find OpenSource applications which code may be reused
* [~] propose secure burn/boot
* [-] propose interface between Device and user device (PC/smartphone)
* [ ] propose device case
* [ ] propose OS (sketch only or RTOS or ...)
* [ ] propose plugins schema in SW
* [ ] propose schematic
* [ ] implement tilt detection in non vertical position, e.g. lying on the sofa
* [ ] implement tilt detection during moving in vehicle 
* [ ] ROADMAP.md: propose MVP and Roadmap
* [ ] CONTRIBUTING.md: development process principals
* [~] TODO.md: plan development
* [ ] TODO.md: estimate development
