# Umicom Notes build practice

Sammy Hegab, Umicom Foundation — MIT

This small complete C23 project teaches the difference between compilation and correct behaviour. It is not a second production editor or document engine. Copy it into a new practice directory before deliberately changing it.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --no-tests=error --output-on-failure
```

Run `umicom-notes-practice "Prepare the workshop note"` from the build folder. Expect four words. In `notes.c`, changing `*outCount = count;` to `*outCount = count + 1U;` still compiles but must fail the tests. Restore the original line and run the tests again. The tests use explicit return values, not Release-disabled assertions.

Input is a caller-owned byte span without embedded NUL. ASCII whitespace separates words. Other UTF-8 bytes belong to a word; this lesson does not implement language-aware Unicode segmentation. No file is read or written by the example program.
