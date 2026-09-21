# Windows deployment and application setup

Umicom Framework owns Windows runtime deployment and setup generation. A thin
application declares its existing executable and identity. Framework prepares
its private runtime and registers it in the installer catalogue. There is no
PowerShell or Python script to distribute or run on an end user's computer.

The supported backend is a **Windows build host with AMD64 applications**, using
one coherent GTK runtime prefix. MSYS2 UCRT64 is the primary development target.
The generated NSIS setup is a native Windows installer that does not link GTK.
This implementation supplies current-user, new-or-empty-folder installation.
Machine-wide installation, elevation, in-place upgrades, repair, silent setup,
online downloads and automatic updates are not implemented by this backend.

## 1. Understand the two build targets

`umicom-windows-runtime` builds the registered executables and refreshes their
private DLLs and resources. Build this target to repair an already-built
executable whose staged DLL was deleted. Newly linked branded executables also
run deployment automatically. An ordinary all-target build includes runtime
refresh targets.

`umicom-windows-installer` depends on the registered GUI applications and runtime
refresh targets. It builds `Umicom-Setup.exe`, `Umicom-Portable.zip`, their SHA-256
files, and a payload inventory. The setup presents application checkboxes, an
**Install all applications** installation type, a destination chooser, and
independent desktop and Start-menu shortcut options.

Only enabled executable targets are offered. A console companion does not appear
as an additional GUI application. The existing `umicom` native command, when
present, is included as a required helper rather than a separate checkbox.
Product availability is not a claim that every feature of that product has
completed its own acceptance testing.

## 2. Configure the existing Windows development build

These examples assume an existing Umicom Applications checkout containing the
Framework submodule. Do not create a second Framework checkout inside it.

Open PowerShell and run:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$ToolRoot = "C:\msys64\ucrt64"
$Cmake = Join-Path $ToolRoot "bin\cmake.exe"
$Ctest = Join-Path $ToolRoot "bin\ctest.exe"
$env:Path = "$ToolRoot\bin;$env:Path"

& $Cmake --preset windows-ucrt64-debug `
    -DUMICOM_WINDOWS_RUNTIME_PREFIX=C:/msys64/ucrt64
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed." }
```

The session PATH above is for the **build tools**, not a workaround that end users
need. The runtime prefix must match the GTK libraries used to link the products.
Do not mix the UCRT64, MINGW64 and CLANG64 prefixes.

When integrating a source update, reconfigure before building. New CMake build
targets do not exist in an old generated build graph until configuration succeeds.

## 3. Prepare click-to-run build output

```powershell
& $Cmake --build --preset windows-ucrt64-debug `
    --target umicom-windows-runtime `
    --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Runtime deployment failed." }

Start-Process explorer.exe `
    -ArgumentList (Join-Path $PWD "build\windows-ucrt64-debug\bin")
```

Double-click an application in that folder. Leave the deployed sibling `lib`
and `share` directories intact. Studio should also be tested from Explorer
started normally, not only as a child of the prepared PowerShell session.

To work on Studio first, building its existing `umicom-studio-ide` executable
target triggers deployment after relinking. The explicit
`umicom-studio-ide-windows-runtime` target also refreshes its runtime without
requiring a source change. The aggregate target above covers all registered
executables.

## 4. Install the setup compiler on the build machine

Install a current supported **NSIS 3 Unicode** compiler from its official
project. This is a build-machine dependency only. Locate `makensis.exe` in the
installed NSIS folder. A common installation path is:

```powershell
$Nsis = "C:\Program Files (x86)\NSIS\makensis.exe"
if (-not (Test-Path -LiteralPath $Nsis -PathType Leaf)) {
    throw "Set `$Nsis to the actual installed makensis.exe path."
}

& $Cmake --preset windows-ucrt64-debug `
    -DUMICOM_WINDOWS_RUNTIME_PREFIX=C:/msys64/ucrt64 `
    "-DUMICOM_NSIS_EXECUTABLE=$Nsis"
if ($LASTEXITCODE -ne 0) { throw "Installer configuration failed." }
```

Some installations use `C:\Program Files\NSIS` instead. Do not download a
standalone DLL to repair the setup compiler; install the complete trusted NSIS
distribution, including its Modern UI, standard includes and plugins.

## 5. Generate and test the setup application

```powershell
& $Cmake --build --preset windows-ucrt64-debug `
    --target umicom-windows-installer `
    --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Installer build failed; do not distribute an older output." }

$PackageFolder = Join-Path $PWD "build\windows-ucrt64-debug\packages\Debug"
Get-ChildItem -LiteralPath $PackageFolder
Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $PackageFolder "Umicom-Setup.exe")
```

The expected outputs are:

| File | Purpose |
| --- | --- |
| `Umicom-Setup.exe` | Self-contained native selection installer. |
| `Umicom-Portable.zip` | All build-selected GUI applications and the same runtime, without installer registration. |
| `Umicom-Setup.exe.sha256` | Checksum of the generated installer. |
| `Umicom-Portable.zip.sha256` | Checksum of the portable archive. |
| `payload-sha256.tsv` | Component, hash and relative path for each packaged payload file. |
| `installer-build.log` | Actual NSIS compiler output, including a failed compilation. |

A failed build must not be mistaken for a new successful package. A previously
published file can still exist in the output folder; always check the current
build exit code and log. The `work` subdirectory is generated build data, not a
folder to distribute as the installer.

To include every available module, enable the suite's existing
`UMICOM_APPLICATIONS_BUILD_ALL_MODULES` option in a suitable checkout. This may
require additional application submodules and build dependencies. The installer
will not fabricate executables for absent or disabled products.

A release should be built in a separate Release directory rather than changing
the established Debug directory in place. For example:

```powershell
& $Cmake --preset windows-ucrt64-debug `
    -B ".\build\windows-ucrt64-release" `
    -DCMAKE_BUILD_TYPE=Release `
    -DUMICOM_WINDOWS_RUNTIME_PREFIX=C:/msys64/ucrt64 `
    "-DUMICOM_NSIS_EXECUTABLE=$Nsis"
if ($LASTEXITCODE -ne 0) { throw "Release configuration failed." }

& $Cmake --build ".\build\windows-ucrt64-release" `
    --target umicom-windows-installer --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Release installer build failed." }
```

That configuration writes its package to
`build/windows-ucrt64-release/packages/Release`. Code signing and public release
approval are separate distributor responsibilities.

## 6. How an application joins the shared implementation

Keep using Framework's existing branding helper in the same CMake directory
that declares the executable:

```cmake
umicom_apply_application_branding(
    TARGET umicom-example
    PRODUCT_NAME "Umicom Example"
    INTERNAL_NAME umicom-example
    APPLICATION_ID "foundation.umicom.Example"
    RESOURCE_ROOT "${UMICOM_FRAMEWORK_RESOURCE_ROOT}"
    WINDOWS_GUI)
```

`WINDOWS_GUI` identifies the frontend for this installer and keeps the normal
Windows GUI-subsystem branding behaviour. No new application-side source file
is required. The shared helper attaches Framework's native runtime bootstrap
sources directly to the executable so their constructor is not discarded from
an otherwise unreferenced static archive.

Useful build-time extension points:

| Setting | Meaning |
| --- | --- |
| `UMICOM_WINDOWS_DEPLOY_RUNTIME` | Defaults to `ON` for this integration. Set `OFF` for a compile-only cross build; that does not produce a click-ready Windows package. |
| `UMICOM_WINDOWS_RUNTIME_PREFIX` | One complete runtime prefix used by the link and deployment environment. |
| `UMICOM_NSIS_EXECUTABLE` | Full path to the build-machine NSIS compiler. |
| `UMICOM_WINDOWS_INSTALLER_TARGETS` | Optional semicolon-separated subset of registered, enabled GUI targets. Unknown, disabled or excluded targets are errors. |
| `UMICOM_WINDOWS_INSTALLER_HELPERS` | Shared executable helpers; defaults to the existing `umicom` target when present. Explicit additional missing helpers are errors. |
| Target property `UMICOM_WINDOWS_INSTALLER_EXCLUDE` | Excludes a registered frontend from the setup catalogue. |
| Target property `UMICOM_WINDOWS_RUNTIME_MODULES` | Absolute DLL paths for additional explicitly known dynamically loaded modules. Their dependencies are inspected too. |

For example, set extra modules with
`set_property(TARGET umicom-example PROPERTY UMICOM_WINDOWS_RUNTIME_MODULES
"${CMAKE_BINARY_DIR}/plugins/example.dll")` and add the correct build dependency
on that module target. A PE import scan cannot discover arbitrary plugin names
constructed later at runtime; the application must declare those modules.

The module also installs its C sources, public cache header, licence, documentation
and CMake support for installed SDK consumers. Framework remains the only source
of implementation in both source-superbuild and installed-SDK use.

## 7. Dependency and resource handling

The scanner inspects the finished executable with CMake's Windows PE dependency
support and the selected `objdump`. It checks the PE machine type, rejects
unresolved imports and conflicting DLLs, and restricts non-system dependencies
to the declared runtime/build roots. It scans an isolated executable copy so a
stale DLL beside the original cannot silently outrank the selected prefix.

GIO modules, GdkPixbuf image loaders and GTK media/print modules are explicitly
included when the prefix supplies them. Schemas, icons, themes, fontconfig data,
licence notices, shared branding and Framework resources are staged as owned
files. Schema and GIO caches are generated from clean, declared source sets, not
from stale modules left in a developer's output directory.

Each report snapshots its executable and payload hashes. Package assembly rejects
files that changed after their report was prepared. Shared destination paths must
have the same spelling and content across applications. Arbitrary SDK archives,
tests and unrelated files from `build/lib` are not copied wholesale into setup.

At startup, Framework locates `bin`, `lib` and `share` relative to the actual
executable. It sets only the application's process environment. Its GdkPixbuf
cache template is expanded into a unique temporary file, cleaned up at normal
process exit. No startup write to the installed directory is required. A killed
process can leave its temporary file for Windows/user temporary-file cleanup.

The bootstrap cannot fix a missing **load-time DLL**: Windows needs that DLL
before executing application code. That is why the build-time deployment step,
not a late PATH assignment inside `main`, is the central fix.

## 8. Focused regression tests

The focused test project does not build the whole Framework or application suite:

```powershell
& $Cmake -S ".\framework\tests\windows_deployment" `
    -B ".\build\windows-deployment-tests" -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug `
    -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe `
    -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe
if ($LASTEXITCODE -ne 0) { throw "Test configuration failed." }
& $Cmake --build ".\build\windows-deployment-tests" --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Test compilation failed." }
& $Ctest --test-dir ".\build\windows-deployment-tests" --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Deployment tests failed." }
```

Portable tests cover cache relocation, bounded writes, path/identifier validation,
PE-header checks, deterministic setup generation, exact refusal diagnostics and
nested/installed-SDK CMake integration. The tiny PE-header fixtures used by those
tests are **not runnable Windows applications**.

A separate real native constructor test is registered only on Windows. Neither a
Linux configuration simulation nor a generated `.nsi` file demonstrates that NSIS
compiled an installer or that a GTK application launched successfully on Windows.

## 9. Clean-Windows release checklist

Use a fresh supported Windows installation without MSYS2, GTK or compiler
folders on PATH. Verify the actual build, not a test-fixture executable.

1. Compile the native startup test and complete the focused tests on Windows.
2. Build the real applications and setup with the selected runtime. Inspect the
   NSIS log, payload inventory and dependency refusals.
3. Install one application, a custom subset, and all included applications in
   separate clean test runs. Verify only the chosen GUI executable files appear.
4. Test all four desktop/Start-menu checkbox combinations. Confirm existing
   unrelated shortcuts are not overwritten.
5. Test paths containing spaces and non-ASCII characters, a writable second
   drive, a non-empty destination, an inaccessible destination and cancellation.
6. Open each application from Explorer and shortcuts without a prepared terminal.
   Check icons, image loading, schemas, fonts, dialogs and relevant plugin features.
7. Extract and relocate the complete portable package. Repeat the launch checks.
8. Exercise normal uninstall, a locked-file retry, and an extra user-created file
   in the destination. Confirm unowned files are retained and failures are shown.
9. Verify release signatures, exact third-party licences, any corresponding-source
   obligations, supported Windows versions and product-specific acceptance tests.

The installer rejects an existing registered installation. This deliberately
avoids pretending that first-install copying is a safe in-place upgrade engine.
To change the application selection, uninstall and reinstall. Projects should
remain outside the application installation directory throughout this process.

## References

- GTK on Windows: <https://www.gtk.org/docs/installations/windows/>
- GTK runtime configuration: <https://docs.gtk.org/gtk4/running.html>
- NSIS scripting reference: <https://nsis.sourceforge.io/Docs/Chapter4.html>
- NSIS Modern UI: <https://nsis.sourceforge.io/Docs/Modern%20UI%202/Readme.html>
- CMake runtime dependency inspection: <https://cmake.org/cmake/help/latest/command/file.html#get-runtime-dependencies>
