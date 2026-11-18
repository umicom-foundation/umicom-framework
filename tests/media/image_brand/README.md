# Focused image-brand checks

![Umicom](../../../resources/brand/umicom-logo.svg)

This small CMake project compiles the production watermark adapter, pixel-surface owner, PNG codecs and native file owners. It runs the same fourteen behavior checks and six fault-injection checks as the full Framework build. It creates no application window and makes no network requests. GTK and its SVG image loader are still required: the test decodes the approved vector artwork rather than substituting a test logo.

## Build from a Framework checkout

1. Install a C23 compiler, CMake, Ninja, GTK4, pkg-config and libpng for the same native toolchain. On Windows, use the UCRT64 tools together. Mixing MSYS and UCRT64 libraries is not a supported shortcut.
2. Open a terminal in the Framework repository. Run each command separately and stop if it fails.

```powershell
cmake -S tests/media/image_brand -B build/image-brand-checks -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/image-brand-checks --parallel 2
ctest --test-dir build/image-brand-checks --output-on-failure
```

3. Read the final test result. A complete run contains twenty passing checks. A skipped PNG case is missing coverage, not a passing PNG export. The focused project requires libpng at configuration time.
4. If SVG decoding fails, inspect the loader installation for the same GTK toolchain. Keep the original error output; replacing the SVG with a bitmap would bypass the behavior being checked.

## What the checks establish

The tests check immutable source pixels, bounded bottom-right placement, the approved compact icon at the minimum size, PNG round-trip, refusal to replace an existing file, cancellation and error cleanup. The fault-injection executable replaces the decoder boundary to make error paths repeatable. The normal executable uses the real decoder and embedded master assets.

Passing this project does not establish that a complete Media application builds, that its windows have been inspected, or that an installed package contains the correct runtime libraries. Run those separate qualification steps for the product being delivered. The target is a source qualification executable, not an installed Framework SDK library.
