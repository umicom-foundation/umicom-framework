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

## Record-level protection

The build fingerprint prevents most stale-object problems, but important
public records also protect themselves at runtime. Two rules are now used:

1. A fixed-capacity array is used when an enumeration can grow. Adding another
   recognised tool no longer changes the size of every toolchain profile.
2. A function that fills caller-owned storage receives the caller's
   compile-time record size. It refuses a different size before reading or
   writing the record.

`UmiToolchainProfile` and `UmiRepositoryWorkflowReport` follow these rules.
Their normal initialisation and execution macros pass `sizeof` automatically,
so application code remains simple. The extra check changes a dangerous memory
overwrite into a clear invalid-argument or link failure.

This matters because one public record is often embedded in several larger
services. A single overwrite can otherwise make toolchain, project, debugger,
repository and Studio tests appear to fail independently even though they all
share one cause.

## How to change a public record safely

Before adding a field or increasing storage:

1. Check whether an existing reserved capacity can hold the new item.
2. Keep growing catalogues independent from the containing record's size.
3. Add or update `structure_size` and `api_version` fields.
4. Validate the caller's size before touching caller-owned memory.
5. Add a test which deliberately passes an undersized record.
6. Reconfigure before building so the public-contract fingerprint is refreshed.

## What a developer should do

After a public C record changes, configure from a fresh cache before building.
This deliberately removes any object compiled against its earlier shape:

```powershell
Set-Location "C:\umicom\umicom-applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --preset windows-ucrt64-debug --fresh
cmake --build --preset windows-ucrt64-debug --parallel 2
```

The configuration output prints a short fingerprint. A changed fingerprint
after a public header update is expected.
