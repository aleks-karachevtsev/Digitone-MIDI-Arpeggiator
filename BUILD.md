# Build and validate

Requirements: Elekloader SDK with `core 4.1.1` and `seq-events 0.1`, GNU m68k ColdFire compiler, and the original verified Digitone MK1 OS 1.44 SysEx. From a directory containing this source tree as `DigiArp-MIDI-DA27-beta`, `sdk/`, `dist/` and `stock/`:

```sh
PATH=/path/to/m68k/bin:$PATH PYTHONPATH=sdk python -m elekloader.sdk.build DigiArp-MIDI-DA27-beta --stock stock/Digitone_and_Digitone_Keys_OS1.44.syx
PYTHONPATH=sdk python -m elekloader.lint DigiArp-MIDI-DA27-beta/out/digiarp-midi-0.9-dev11.elemod --stock stock/Digitone_and_Digitone_Keys_OS1.44.syx --with dist/core-4.1.1-os1.44.elemod --with dist/seq-events-0.1-os1.44.elemod --json
```

The supplied binary's SHA-256 is `b82506a6adb3c39545eb6daa731e5e28d09e6c61a07dcedb312a65066b84ff58`. The source was copied from the exact DA27 tree; build reproducibility is recorded separately in `TEST_RESULTS.md`.
