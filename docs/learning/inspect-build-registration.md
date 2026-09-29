# Inspect the files and tests in a configured build

The workflow below is also extended by two optional lessons:
[compile declared public headers](compile-public-headers.md) and
[review required and optional tests](review-test-requirements.md).
The original five-header list later in this lesson records the initial inventory
declarations; the compiler-check lesson describes the additional reviewed owners.
The original `compare` command retains its strict behaviour. Policy assessment
is a separate explicit command and does not change earlier comparison results.

A configuration chooses which parts of Framework to build. For example, a small
command-line lesson and the complete Applications workspace use different sets
of targets. A target is a library, executable or build task known to CMake.

This lesson records those choices and compares the test names that CMake knows
with the names that CTest can discover. A successful comparison means the names
agree and no discovered test is disabled or missing its command. You still need
to run the tests, inspect skips and failures, and review the release requirements.

The inventory service is part of `Umicom::distribution`. The command-line tool
only reads files and prints results. CMake collects build information because it
owns the configured target graph; CTest supplies its own registration JSON. The
existing release-baseline contract and evidence commands continue to work.

## 1. Configure a small learning build

Start PowerShell in the Framework checkout. These commands assume MSYS2 UCRT64
is installed in `C:\msys64`. Keep the build directory separate from the source.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
& "C:\msys64\ucrt64\bin\cmake.exe" `
    -S ".\tools\release_baseline" -B ".\build\inventory-lesson" -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
    -DUMICOM_RELEASE_INVENTORY_CAPTURE=ON
if ($LASTEXITCODE -ne 0) { throw "Configuration failed." }

& "C:\msys64\ucrt64\bin\cmake.exe" --build ".\build\inventory-lesson" --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Build failed." }
```

The focused project compiles the actual Framework distribution implementation.
It is a dependency subset, so it does not qualify the full Framework SDK. To
inspect an Applications build, add `-DUMICOM_RELEASE_INVENTORY_CAPTURE=ON` to
that workspace's normal configure command and use its binary directory below.
Capture defaults to off; the library and inspection tool remain available.

Configuration writes `release-inventory/inventory-Debug.tsv` in the binary
directory. TSV means tab-separated values. Its strings are hexadecimal encoded
so paths, tabs and generator expressions cannot change the record boundaries.
Use the inspection tool to read it in ordinary text.

## 2. Read the configured inventory

```powershell
$Build = (Resolve-Path ".\build\inventory-lesson").Path
$Tool = Join-Path $Build "bin\umicom-release-inventory.exe"
$Configured = Join-Path $Build "release-inventory\inventory-Debug.tsv"
& $Tool show $Configured
if ($LASTEXITCODE -ne 0) { throw "Inventory could not be read." }
& $Tool list $Configured
if ($LASTEXITCODE -ne 0) { throw "Inventory listing failed." }
```

`show` reports totals. `list` prints each header, target, source/target pair and
registered test. Console control characters are printed as `\xNN` escapes.
The output deliberately keeps unknown ownership visible:

| Record | Meaning |
| --- | --- |
| Header, `declared` | A configured target explicitly declares ownership of this public header. |
| Header, `unassigned` | No target has declared ownership. Review is still needed. |
| Target, `configured` | A non-imported build target exists in this configuration. Its detail contains raw target properties. |
| Source, `present` | A direct source path exists and its SHA-256 digest was read during configuration. |
| Source, `unresolved` | The entry uses a generator expression, is generated, or cannot be resolved to an existing direct source file. |
| Test, `registered` | A name was registered. This says nothing about whether its test passes. |

All `.h` files below Framework's `include/umicom` are enumerated, even in the
focused build. This is an on-disk public-header population, not a list of headers
promised by the focused installed package. The initial declarations cover
`base/status.h` and distribution's runtime `types.h`, `release_gate.h`,
`evidence.h` and `inventory.h`. Other headers remain unassigned until reviewed.
Header ownership does not prove symbol ownership, self-contained compilation,
ABI compatibility or lifetime correctness.

After reviewing a header and its implementation, its real CMake target can add
an explicit declaration using a path relative to Framework's root:

```cmake
umicom_release_inventory_own_headers(umicom_distribution
    include/umicom/distribution/runtime/inventory.h)
```

Declarations on aliases, imported targets, missing files, files outside the
public tree or conflicting owners are refused. Ownership is a build declaration,
not a substitute for an engineering review of the public API.

## 3. Capture CTest's registered tests

Build first so CTest can find the executables. Then collect its registration
metadata. This command does not run a test.

```powershell
& "C:\msys64\ucrt64\bin\cmake.exe" "-DBUILD=$Build" -DCONFIG=Debug `
    -P ".\cmake\UmicomCaptureCTestInventory.cmake"
if ($LASTEXITCODE -ne 0) { throw "CTest registration capture failed." }

$Observed = Join-Path $Build "release-inventory\ctest-Debug.tsv"
& $Tool show $Observed
if ($LASTEXITCODE -ne 0) { throw "CTest inventory could not be read." }
& $Tool compare $Configured $Observed
$ComparisonExit = $LASTEXITCODE
if ($ComparisonExit -eq 1) { throw "Review missing/added names, disabled tests and missing commands." }
if ($ComparisonExit -ne 0) { throw "Comparison input or context is invalid." }
```

The adapter retains CTest's JSON and diagnostics beside the TSV. Each test's
detail includes its command as a JSON argument array, selected properties such
as labels and working directory, and a registration location when available.
Environment properties are not copied into the TSV. A disabled test and a test
without a command are explicit states; a test with both conditions contributes
to both totals. `required-policy=unassigned` means that a product owner has not
classified the test as required or optional through this inventory format.

The comparator reports individual added and missing names. Equal totals alone
cannot pass. A comparison requires the same source root, binary root,
configuration and generation identifier. Reconfiguring creates a new generation;
capture CTest again afterwards. Do not configure concurrently with capture.

Exit code 0 means an inspection completed, or the compared registration has
matching names with no disabled tests or missing commands. Exit code 1 means a
registration concern needs review. Exit code 2 means invalid input, mismatched
context, empty test populations or an I/O error. None of these codes certifies a
release or changes the existing release-baseline evidence ledger.

## 4. Run the tests separately

```powershell
& "C:\msys64\ucrt64\bin\ctest.exe" --test-dir $Build `
    --parallel 2 --no-tests=error --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Tests failed." }
```

Read the complete result, including skipped or disabled tests. For a full
Applications configuration, also perform that product's complete build, native
journeys and installed SDK checks. Passing this small learning build cannot
replace them. The inventory tests cover parsing errors, output preservation,
4,097 distinct test names, exact differences, context mismatch and CMake's late
registration hooks. Allocation-failure checks are registered on Linux with a
GNU-compatible linker.

## 5. Try the installed public API

This separate consumer finds the focused installed package rather than including
the source tree. Use a fresh installation prefix for the exercise.

```powershell
$Install = Join-Path $Build "installed"
& "C:\msys64\ucrt64\bin\cmake.exe" --install $Build --prefix $Install
if ($LASTEXITCODE -ne 0) { throw "Installation failed." }
& "C:\msys64\ucrt64\bin\cmake.exe" `
    -S ".\examples\release_inventory_consumer" -B ".\build\inventory-consumer" `
    -G Ninja -DCMAKE_BUILD_TYPE=Debug "-DCMAKE_PREFIX_PATH=$Install"
if ($LASTEXITCODE -ne 0) { throw "Installed consumer configuration failed." }
& "C:\msys64\ucrt64\bin\cmake.exe" --build ".\build\inventory-consumer" --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Installed consumer build failed." }
& "C:\msys64\ucrt64\bin\ctest.exe" --test-dir ".\build\inventory-consumer" --no-tests=error --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Installed consumer failed." }
```

## What the capture does and does not cover

Configured build targets are collected after deferred CMake completion hooks.
Imported targets and aliases are not separate build-target records. Disabled
feature branches do not create targets. Raw link/include/compile properties are
recorded before generator evaluation; they are not the final compiler command
lines. Use the build's compiler command database for those commands.

Generated and expression-based source inputs remain unresolved. CMake list
expressions may appear as several source entries; the target record retains the
complete raw property. Public headers and resolved direct sources carry digests
of bytes read at configure time. Digests do not lock the working tree, verify
Git cleanliness, authenticate authorship or cover every resource and dependency.
Reconfigure after changes when collecting qualification evidence.

The CMake name population includes names registered with `add_test`. A test
created only by a CTest include script, or a configuration-specific registration
filtered out by CTest, can therefore produce a deliberate mismatch requiring
review. Duplicate names across directories are refused as ambiguous by the
native parser. Repeated source-directory instances that cannot be traversed
unambiguously are refused during capture. Semicolons in root paths and literal
semicolons in CMake test names are not supported by this collection adapter.
Do not rename existing project APIs or tests merely to hide an inventory gap.

For a multi-configuration generator, select the same configuration when building
and capturing. With an empty `CMAKE_BUILD_TYPE` on a single-configuration
generator, the inventory filename and capture argument use `NoConfig`.

The native parser limits one input to 64 MiB, 200,000 records and 256 KiB per
decoded string field. It rejects excess input rather than truncating it. Test,
header and target identities must be unique; one source can belong to several
targets. The format starts with `UMICOM-RELEASE-INVENTORY<TAB>1`, then a context
row containing producer, generation and hex-encoded roots/configuration. Data
rows contain kind, hex identity, hex owner, plain state and hex detail. The parser
preserves string bytes; it does not certify UTF-8 validity. Read-only API views
remain valid until the owning inventory is destroyed.

If configuration fails, read its first error before using any older inventory.
If CTest reports missing commands, complete the build and recapture. If context
differs, use files from the same build and configuration, then recapture. If a
comparison reports unassigned owners or unresolved sources in inspection output,
retain those entries for review. Store generated inventories and logs with local
build evidence, outside public source documentation.
