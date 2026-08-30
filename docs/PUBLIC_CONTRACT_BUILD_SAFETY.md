<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/PUBLIC_CONTRACT_BUILD_SAFETY.md

PURPOSE:
Explain how Framework protects incremental builds from stale public-header
objects and why this prevents apparently unrelated stack corruption.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
---------------------------------------------------------------------------- -->

# Public Contract Build Safety

## Why this protection exists

A C header describes the shape of data shared by many source files. Imagine
that one source file thinks a toolbox has 22 drawers while another source file
thinks the same toolbox has 26. If the newer code fills all 26 drawers in the
older, smaller toolbox, it writes beyond the space that was reserved. That can
cause a crash even though both source files look correct when read separately.

Normally, Ninja notices that a header changed by looking at the file's modified
time. A copied file, restored backup, archive or merge can sometimes contain
new text while keeping an older time. An old object file can then be linked to
a newly compiled library.

## What Framework now does

During CMake configuration, Framework:

1. Finds every public header below `framework/include/umicom`.
2. Calculates a SHA-256 hash from each header's path and contents.
3. Combines those hashes into one public-contract fingerprint.
4. Adds the fingerprint to the compile command inherited by Framework clients.

When any public header changes, the compile command changes. Ninja must then
recompile every affected Framework library, test and client application even if
the copied header has an unreliable timestamp.

This is a safety net, not a replacement for ordinary dependency tracking. CMake
and Ninja still use their normal fast incremental build behaviour.

## What a developer should do

Run the normal CMake configure command before building. No special cleanup or
manual deletion of object files is required:

```powershell
Set-Location "C:\umicom\umicom-applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --parallel 2
```

The configuration output prints a short fingerprint. A changed fingerprint
after a public header update is expected.
