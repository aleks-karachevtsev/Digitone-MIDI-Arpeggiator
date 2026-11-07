# DigiArp MIDI DA27 beta

Experimental MIDI arpeggiator for Elektron Digitone MK1, OS 1.44, with independent settings for MIDI tracks A–D. This is the exact DA27 module the user installed and tested on physical hardware. It is the current rollback baseline while the remaining single-note pitch anomaly is investigated. It is not a stable public release.

The existing Digitone ARP page controls the MIDI arpeggiator. The mod supports the native graphical step editor, MIDI Note On/Off, tempo sync, recorded MIDI trigs, original modes and UP TWICE, DOWN TWICE, UP/DOWN REPEAT. The synth tracks T1–T4 keep their stock ARP path. LATCH is not implemented.

- [Hardware controls](HARDWARE_CONTROLS.md)
- [Installation](INSTALL.md)
- [Building](BUILD.md)
- [Test results](TEST_RESULTS.md)
- [Known issues](KNOWN_ISSUES.md)
- [Changelog](CHANGELOG.md)

`DigiArp-MIDI-DA27-0.9-dev11.elemod` requires Elekloader core 4.1.1 and seq-events 0.1, plus a verified stock Digitone OS 1.44 image when constructing a firmware update. Do not send the `.elemod` directly to the hardware as an OS SysEx image. Proprietary Elektron firmware and dependency binaries are not included.

License: GPL-2.0-or-later. See `LICENSE`. This is an experimental beta release. See GitHub Releases for the installable module and KNOWN_ISSUES.md for its limitations.
