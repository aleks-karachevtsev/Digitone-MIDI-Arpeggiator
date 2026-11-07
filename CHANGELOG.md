# Changelog

## DA27 / 0.9-dev11 — experimental hardware-tested baseline

- Connected native graphical pattern length to MIDI playback; active steps wrap at lengths 1–16.
- Retained independent MIDI ARP enabled state across non-OFF MODE edits and fixed the OFF shortcut transition.
- Tracked live key ownership across MIDI track changes and rejected sequencer trigs after STOP.
- Preserved MIDI A–D settings, graphical step masks and pitch offsets, note scheduling, shortcut indicator, and stock synth-track path.
- Passed Elekloader lint, host and full emulator tests. Installed and tested by the user on physical hardware; unresolved single-note pitch anomaly remains.

Earlier hardware-tested DA25 remains an older rollback. DA28 musical SPEED behavior has not been built.
