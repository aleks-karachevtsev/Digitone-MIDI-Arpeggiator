# Reproducing the recorded tests

The C engine tests in this directory can be built against `src/digiarp.c` with a host C compiler, for example:

```sh
gcc -O2 -I. src/digiarp.c tests/test_regression26.c -o /tmp/digiarp-test
/tmp/digiarp-test
```

The Python full-device emulator tests require a separately installed Digitone emulator, the verified stock OS and the dependency modules. Build the combined DA27 SysEx with Elekloader's `patch` command, place `DA27.syx` and its `.map.json` in `test-artifacts/`, create `test-artifacts/`, and run from the repository root with the emulator on `PYTHONPATH`. The saved `validation/` logs are the results actually obtained in the development environment. These tests cannot run from this archive alone because proprietary OS files and the emulator are not redistributed.
