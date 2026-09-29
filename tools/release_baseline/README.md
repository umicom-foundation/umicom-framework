# Native Core release baseline

Build from this directory to exercise the actual distribution gate, new named-evidence adapter and retained platform thread/queue sources. This is a focused dependency subset. The production CMake include attaches the same implementation to `Umicom::distribution`; there is no alternate application runtime.

```
cmake -S framework/tools/release_baseline -B build/fw01-baseline -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/fw01-baseline --parallel 2
ctest --test-dir build/fw01-baseline --parallel 2 --no-tests=error --output-on-failure
```

Read `docs/releases/core/EVIDENCE_FORMAT.md` before supplying assertions. `show` displays missing requirements. `check` returns one when release evidence remains blocked. Neither command mutates its inputs or opens referenced logs. Use ordinary trusted files; the CLI is not an input-filesystem sandbox and network-mounted paths can still cause filesystem network access.

The full product build and native GUI checks remain separate required gates. Keep the staged package, user data and external services out of the practice test environment.
