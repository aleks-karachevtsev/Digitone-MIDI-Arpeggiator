# Known issues and limits

- On physical hardware, one held key with transport stopped, RANGE=1 and zero graphical offsets can sometimes produce an unintended repeating sequence of different pitches. The cause is not established. The emulator's exact MIDI traces did not reproduce it. Do not describe this as fixed.
- Press-and-turn musical SPEED navigation for encoder B is not implemented. At the stock setter, pressed turns were observed to jump by four values and some single turns were coalesced. An upstream input hook needs validation before changing DA27.
- LATCH is not implemented.
- The user has installed and tested DA27 on a Digitone, but individual feature-level hardware pass results and an external MIDI byte trace were not supplied for this package. Emulator results do not establish hardware correctness.
