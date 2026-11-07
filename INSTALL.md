# Installation for a hardware beta test

Save projects and keep a copy of the currently installed working DA27 image before updating. In Elekloader select the verified original Digitone MK1 OS 1.44 SysEx, then add **core 4.1.1**, **seq-events 0.1** and `DigiArp-MIDI-DA27-0.9-dev11.elemod`. Validate the combined image and install it using Elekloader's normal Digitone update procedure. The `.elemod` is a module, not an update image by itself.

This package contains neither Elektron's OS nor core/seq-events binaries. The known stock OS 1.44 SHA-256 is `d4f200d04484333d82822db7744e6484d0def8f2db8ddf55ee2b780cc13c9659`.

Check MIDI A–D with the sequencer stopped and running. Start with RANGE=1, all offsets zero and one held key; compare actual outgoing pitches with that key. Check a chord after releasing only its middle note, graphical lengths 4 and 7, the ARP shortcut and LED across tracks, then verify ordinary synth behavior on T4. The single-key pitch anomaly remains open; retain a way to restore the already working image.
