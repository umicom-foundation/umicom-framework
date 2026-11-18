# Provider text boundary checks

![Umicom](../../resources/brand/umicom-logo.svg)

These tests compile the real chat and avatar-request codecs with the native UTF-8 validator and JSON services. They do not load credentials, send prompts, start models or call a remote provider. They exercise a compatibility rule: malformed UTF-8 in supported text fields returns `UMI_STATUS_PARSE_ERROR`, while missing, unterminated or forbidden-control input returns `UMI_STATUS_INVALID_ARGUMENT`.

## Run from a Framework checkout

Use a C23 compiler, CMake and Ninja. Run each command separately and stop on an error.

```powershell
cmake -S tests/provider_text -B build/provider-text-checks -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/provider-text-checks --parallel 2
ctest --test-dir build/provider-text-checks --output-on-failure
```

Seven cases cover five text positions, valid multilingual/escaped input and field admission. The malformed cases include overlong encodings, a surrogate, an out-of-range code point, isolated continuation bytes, truncated sequences and a wrong continuation byte. They also compare a saved copy of each initialized plan to ensure rejection happens before the codec changes it.

The shared JSON writer intentionally has its own lower-level status contract. The test checks that contract too, so replacing a writer cannot silently change the provider-facing classification. This does not make all invalid input interchangeable: an unsupported operation or missing required value is still a field error.

These source-level checks do not replace the public plan lifecycle, credential, transport or application tests. The full Framework test graph registers the same new cases against the normal libraries.
