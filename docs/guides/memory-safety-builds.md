# Check C memory safety with sanitizers

A sanitizer adds checks to a development executable. AddressSanitizer can detect many out-of-bounds accesses and uses of released memory. UndefinedBehaviorSanitizer can detect selected invalid C operations. These checks report problems on paths actually exercised by tests; they do not make C23 memory-safe or prove that every path is correct.

Antivirus detection is a separate investigation. Turning on a sanitizer, adding a signature or obtaining a passing test result does not establish that a quarantined executable is safe.

## Choose a separate build folder

Keep your ordinary build and instrumented build separate. This avoids mixing objects built with different runtime requirements. Use a GCC or GNU-style Clang toolchain with both sanitizer runtimes installed. Some Windows toolchains do not provide them. The Framework checks compilation and linking when `UMICOM_ENABLE_SANITIZERS=ON` and stops configuration with a useful message if that combination is unavailable.

MSVC's different sanitizer support is not enabled by this combined address/undefined mode. It is rejected explicitly rather than silently providing no coverage. Do not copy a sanitizer runtime from an unrelated compiler installation.

## Configure, build and test

From the Framework repository in a supported host environment, run these commands one at a time. Stop if a command fails:

~~~text
cmake -S . -B build/sanitizers -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DUMICOM_ENABLE_SANITIZERS=ON
cmake --build build/sanitizers --parallel 2
ctest --test-dir build/sanitizers --output-on-failure --no-tests=error --output-log build/sanitizers/tests.log
~~~

Replace `clang` with the path to your supported compiler if needed. The configure probe links a small executable but does not run it. Successfully configuring therefore confirms compiler/linker support, not runtime qualification.

The `framework.build.sanitizers.static-link` test checks that an instrumented static library carries its runtime requirements to a consumer. The disabled and unsupported-compiler tests check ordinary builds and clear refusal. The static-link test is registered only when sanitizers are enabled; its absence in an ordinary build is not a pass.

On Windows, keep the existing ordinary UCRT64 preset when that toolchain cannot supply the runtimes. A separate supported environment can provide additional sanitizer coverage. Keep normal antivirus protection enabled in both environments.

## Investigate a report

1. Keep the complete diagnostic, test name, source revision, compiler version and command.
2. Find the first invalid access and its allocation/release history. A later crash may be a consequence.
3. Correct ownership, bounds or lifetime in the shared component responsible for the data.
4. Add a regression that exercises the failing input or callback order.
5. Rebuild the affected targets and rerun both that regression and related tests. Then repeat the wider qualification before release.

For GTK code, a signal handler can close a window or replace a model before the caller resumes. Retain objects needed across callbacks, reacquire application authority afterwards, and release text-input focus while its native surface still exists. Suppressing a critical diagnostic is not a repair.

## Keep results precise

Keep failed, skipped and unavailable tests separate. Save logs outside public repositories when they include local paths or private data. Ordinary builds remain useful, but do not describe a run with sanitizers disabled as memory-safety validation.
