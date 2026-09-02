<!--
Umicom Framework
File: docs/REPOSITORY_SOURCE_DOCUMENTATION.md

PURPOSE:
Explain the repository-wide source documentation pass, its safety rules and
the repeatable audit used to keep future Framework and application code clear.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Repository Source Documentation

Umicom source code is part of the learning material. A new contributor should
be able to open a file and discover who owns it, why it exists, what each
function provides and why an important branch or loop is needed.

The repository documentation audit covers project-owned C, C++, public
headers, reusable include fragments, assembly, Rust, Zig, CMake, PowerShell,
shell, CSS and SVG files. JSON is not included because the JSON format does not
allow comments. Generated build output and imported dependency folders are
also excluded because Umicom does not own those files.

## What the complete pass records

The repository-wide pass reviewed 22,224 source and script files and added
142,723 missing headers or nearby explanations to 20,361 files. Existing
comments, source statements, function names and public contracts were kept.
The update did not remove features or execute application code.

Each covered file now provides:

- a standard ownership header with its file path, purpose, author,
  organisation and licence;
- a nearby explanation for each detected C or C++ function;
- structured comments for public structures, enumerations and functions;
- an explanation beside each detected condition, loop, switch and fallback;
- function and decision guidance in build and contributor scripts;
- language-native comments for the small assembly, Rust and Zig template set;
- retained design and accessibility comments in CSS and SVG resources.

Comments explain a rule or relationship that syntax cannot explain by itself.
Names remain descriptive. The word `count`, for example, is not shortened just
because a comment now explains how the count is bounded.

## Running the audit

On Windows, run the read-only audit from the suite root:

```powershell
Set-Location "C:\umicom\umicom-applications"

powershell -NoProfile -ExecutionPolicy Bypass `
    -File ".\scripts\umicom-document-source.ps1" `
    -Mode Audit `
    -IncludeTemplates
```

The command exits with a failure when documentation is missing. It does not
compile code, start an application or change a file.

The same check is available through the manually selected CMake target:

```powershell
& "C:\msys64\ucrt64\bin\cmake.exe" `
    --build `
    --preset windows-ucrt64-debug `
    --target umicom-source-documentation-audit
```

The target is separate from the normal product build because a complete source
scan is intentionally thorough and can take time in a large checkout.

## Repairing a documentation gap

Apply mode inserts missing comments but never deletes existing prose or code:

```powershell
Set-Location "C:\umicom\umicom-applications"

powershell -NoProfile -ExecutionPolicy Bypass `
    -File ".\scripts\umicom-document-source.ps1" `
    -Mode Apply `
    -IncludeTemplates
```

A focused review can use `-PathPrefix` with a repository-relative file or
folder. The contributor should read every generated explanation, improve it
when domain knowledge can make it more precise, and include the comment in the
same review as the code it describes.

## Relationship to the living HTML reference

The audit protects the explanations stored beside source. The
`umicom-documentation` target reads structured public comments and Markdown
guides to generate a navigable local HTML reference. The generated pages stay
in the build folder, while the reviewed comments remain the source of truth.

The source audit, CodeGuard coverage reports and HTML reference therefore have
different jobs:

1. the source audit finds missing explanations across the checkout;
2. CodeGuard exposes documentation evidence to applications and quality gates;
3. the HTML target turns reviewed contracts into a searchable reference.

When behaviour changes, its nearby comment must change in the same commit. A
comment that no longer describes the code is a defect and should be corrected
rather than preserved unchanged.
