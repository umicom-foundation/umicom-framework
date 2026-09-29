# Compile public headers as independent consumers

A source file can compile because another include happened to declare a type
first. A developer using the public header in a different order then sees an
error. Independent header checks expose that hidden dependency.

These checks complement Framework's existing header governance tool. Governance
checks comments, include guards and source contracts. The new compiler checks
generate a small C23 source file for each explicitly declared public header,
include that header twice, and compile it using the owner's public CMake usage
requirements. No preparatory system include or private include directory is
added. Unity builds and precompiled headers are disabled for the probes so one
header cannot accidentally prepare another header's declarations.

## 1. Enable the checks

Add `UMICOM_RELEASE_HEADER_CHECKS=ON` to your usual configuration. For the
Applications workspace on Windows:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
& "C:\msys64\ucrt64\bin\cmake.exe" `
    --preset windows-ucrt64-all-debug -DBUILD_TESTING=ON `
    -DUMICOM_BUILD_NATIVE_TOOL=ON -DUMICOM_BOOT_MEDIA_ENABLE_DEVICE_WRITES=OFF `
    -DUMICOM_RELEASE_INVENTORY_CAPTURE=ON -DUMICOM_RELEASE_HEADER_CHECKS=ON
if ($LASTEXITCODE -ne 0) { throw "Configuration failed." }
```

Configuration prepares the checks; it does not compile them. A message showing
a header count is not a passing result. The checks default to off, and enabling
them does not silently change the normal build's target population to include
all probe compilations.

## 2. Build the dedicated target

```powershell
& "C:\msys64\ucrt64\bin\cmake.exe" --build `
    --preset windows-ucrt64-all-debug --target umicom-public-header-checks --parallel 2 -- -k 0
if ($LASTEXITCODE -ne 0) { throw "A declared public header failed its consumer compilation." }
```

Inspect `build/windows-ucrt64-all-debug/release-inventory/header-checks/manifest.tsv`
to see each header, owner and compile target. Generated C files live in the
binary tree. They are not public source files and should not be committed.
The build can also build owner dependencies needed by CMake's target graph.

If a header fails, use the first compiler diagnostic to find its missing public
include or incompatible declaration. Fix the dependency where it belongs,
retaining existing APIs and comments. Do not add a blanket preparatory include
to every probe just to make the checks pass.

## 3. Understand which headers are covered

The release inventory still enumerates all `.h` files below `include/umicom`.
Only explicitly declared headers receive these compiler probes. Unassigned
headers remain visible and are not counted as compiled.

The declared set extends the original distribution and status contracts with
the test-policy API and reviewed pairs in base, platform and runtime services.
It also covers the native source-contracts public header when that owner is
configured. For each reviewed pair, the real non-imported target must actually
contain the canonical implementation source. A focused build that omits an
implementation does not acquire its header merely because a similarly named
target exists. The type-only `base/version.h` belongs to the configured base
contract.

This is build ownership, not a completed symbol-by-symbol API/ABI review. These
checks compile declarations; they do not link every public function, prove
binary compatibility, exercise behaviour or inspect every possible include
order. The full unassigned population still requires review. An installed
package needs its own consumer checks because a source-tree include path can
contain headers that were not installed.

## 4. Add a reviewed ownership declaration

The target must already exist. Use a path relative to Framework's root:

```cmake
umicom_release_inventory_own_headers(umicom_distribution
    include/umicom/distribution/runtime/test_policy.h)
```

Capture waits for deferred composition hooks, then gathers declarations and
creates the probes. Imported targets, aliases, missing public files and
conflicting owners are rejected by the ownership declaration helper. Preserve
the decision's engineering rationale near the declaration; header filenames
alone do not establish ownership.

## 5. Check your own library or an installed owner

The lower-level helper is reusable without the Framework inventory. It is
installed in `share/umicom/cmake/UmicomPublicHeaderCompileChecks.cmake`. Include
it after creating or importing your owner target:

```cmake
include("${SDK_PREFIX}/share/umicom/cmake/UmicomPublicHeaderCompileChecks.cmake")
umicom_add_public_header_compile_check(my-library-public-check
    MyLibrary::public "${MY_LIBRARY_INCLUDE_ROOT}" "my_library/public.h")
```

`SDK_PREFIX` and `MY_LIBRARY_INCLUDE_ROOT` are ordinary variables you set to
your actual installation and public include directory. Build the named target
with `cmake --build <binary-directory> --target my-library-public-check`.
The helper accepts imported owners because it tests consumer requirements; it
does not register or claim ownership of their headers. It never injects the
supplied include root into the compile options: the owner must publish the
include path through its public interface.

The regression fixtures include a valid public header and a deliberately
undeclared type in another header. The valid probe must build; the invalid probe
must fail with that type in its diagnostic. Those expected failures live only
in isolated fixture build directories and do not modify the Framework source.
