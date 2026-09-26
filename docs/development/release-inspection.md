# Native Windows release inspection

Sammy Hegab · Umicom Foundation · MIT

## Ownership

`Umicom::release_inspector` consumes the existing Setup Centre catalogue,
receipt, selected-file rules, path validation, bounded text builder, checked
filesystem operations and Native Launcher SHA-256 implementation. It neither
runs a shell nor loads a dependency. The Win32 adapter dispatches inspection
on its existing worker. GUI access remains on the window thread.

`UmiSetupInstalledBundleOpen` is the additive, metadata-only companion to
`UmiSetupBundleOpen`. It reuses the same private decoder with the installed
receipt schema selected. Existing interfaces and structure layouts are unchanged.

## API map

| Entry point | Contract |
|---|---|
| `UmiReleasePeInspect` | Parse one bounded memory image. Reset output on any failure. No filesystem or loader calls. |
| `UmiReleaseInspectBundle` | Read selected/shared files from an already opened release or receipt. The caller keeps the bundle alive. |
| `UmiReleaseInspectInstallation` | Open the canonical installed receipt, inspect and release that bundle. |
| `UmiReleaseInspectionSummary` | Render a bounded one-line summary; insufficient capacity yields an empty string and failure. |
| `UmiReleaseInspectorMain` | Common CLI implementation used by the small native entry point. |

The memory parser understands PE32 and PE32+ metadata. Release inspection
intentionally qualifies only AMD64 PE32+ images with GUI or console subsystem.
DLL import names are restricted to ASCII letters, digits, hyphen, underscore
and full stop, ending in `.dll` or `.drv`. Private dependency resolution uses
selected inventory entries in `bin/`. This is an explicit release profile,
not a general-purpose Windows loader emulator.

## Evidence and limits

`complete` means enumeration completed, not that no issue was found. `status`
remains the return status. `runtimeTested` is always zero. A private-import
observation describes catalogue membership; the target's content and metadata
are checked as separate observations. System/API-set names are classified as
deferred host requirements. The visible system-name list is not claimed to be
Windows' KnownDLLs registry. Unknown names require a private file or explicit
engineering review; there is no PATH fallback.

The standalone `Umicom-Setup.exe` copy must agree with its catalogue payload
copy when the native bootstrap is present. That bootstrap may not import a
private DLL: it sits outside `payload/bin` before installation. Inspecting the
bootstrap is not authenticating the publisher or validating its signature.

Each file is observed once. Catalogue/receipt bytes are rechecked at both
ends. This does not lock every input against later changes. Installation still
performs its canonical fingerprint/copy verification. Unlisted personal files
are not inspected or removed. Likewise, this is not a strict recursive scan
of extra files a third party has added to an installation.

Bounds: 256 MiB per PE image, 96 sections, 512 import descriptors per image,
256 bytes per DLL-name buffer and 262,144 emitted observations. JSON uses the
existing Setup Centre bounded text writer. Only one image buffer is retained
at a time. These limits deliberately fail closed and are not silently raised
for a damaged input. The underlying catalogue's own file/size limits also apply.

The parser verifies header/section ranges, image mapping, import descriptors,
DLL names and bounded IAT/name-table addresses. It does not walk every imported
symbol, validate all PE directories, validate relocations or executable code,
resolve forwarded exports, prove required CPU instructions, check signatures,
or discover every LoadLibrary/plugin name. Old VA-based delay descriptors are
reported as unavailable rather than misread as RVAs.

## Callbacks, cancellation and lifetime

A callback runs synchronously on the calling worker and receives borrowed
strings. Copy anything that must survive it. Do not mutate the bundle, re-enter
its inspection, free caller state or update widgets from that callback.
A nonzero callback result requests cancellation. Empty-path FILE_CHECKED
callbacks are quiet hash-progress heartbeats, not completed-file observations.
They are excluded from result counts and JSON output.

Cancellation is observed between PE reads and during streaming non-PE hashes.
A single bounded `ScRead` is not interrupted halfway through. The window waits
for worker completion before freeing its job or bundle. The existing process
runner remains the only owner of child-process launch and cancellation; this
library never calls it.

## Path repair and preserved implementations

The original `Component` function stopped examining the component when it
encountered its first full stop. That admitted reserved characters in a suffix.
The replacement scans the entire component before checking its basename
against reserved device names, including COM/LPT superscript digits. The old
function is retained inside a documented `#if 0` block. Accepted ordinary UTF-8
names and multiple full stops remain covered by regression tests.

The older GUI review/install/verify calls remain in disabled review blocks.
The active GUI adds static inspection and delegates the actual transaction to
the same existing install service. Shortcut-checkbox changes invalidate the
visible review. Fonts are recreated for the current DPI with explicit GDI
ownership; this source still requires actual Windows compilation and UI tests.

## Build integration

`UmicomSetupCentre.cmake` includes `UmicomReleaseInspector.cmake` before the
native packaging helper. The latter carries the inspector and public lesson
as shared payload files, then runs the native inspector after the packer.
A failed inspection makes `umicom-native-installer` fail and leaves its newly
created output for review. There is no automatic deletion or publication.
The previous NSIS route and earlier scripts remain untouched alternatives.

The Windows Notes release laboratory is optional through
`UMICOM_RELEASE_NOTES_EXAMPLE`. It is not registered as a first-party installer
application. Its own small teaching DLL does not replace the production
Framework document model. The three Windows acceptance tests use that real
DLL/application pair, the canonical packer/installer and the canonical process
runner. They are not registered on a non-Windows host.

Clang/LLD cross-format fixtures are optional host-build tests. They generate
real PE records without a Windows SDK, but are never executed. The deliberately
inert delay helper is sufficient for metadata construction only. A successful
cross-format check does not qualify the Win32 adapter or Windows startup.

## Reproduction and acceptance

The focused composition is `examples/release_inspector`, with
`UMICOM_SETUP_CANONICAL_PROCESS=ON` to include the actual platform process files.
The `umicomOS/tools/release-inspector` entry delegates to that composition.
The standalone installed package is a dependency-subset SDK, not the complete
Framework export. Use the full Applications preset for integration acceptance.

The release checklist remains: compile the actual Windows targets; run native
and hidden-control tests; package; inspect; install under a standard user on a
clean machine; run every advertised application from its installed shortcut;
exercise real workflows; record high-DPI and close-during-work results.
Unrun steps remain unrun even when the portable suite is green.

## Primary technical references

- Microsoft PE format: https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
- Windows names and paths: https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
- Windows DLL search order: https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
- Error-mode inheritance used by the single-threaded Windows test: https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-seterrormode
