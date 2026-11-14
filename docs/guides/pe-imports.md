<!--
Umicom Framework
File: framework/docs/guides/pe-imports.md

PURPOSE:
  Inspect PE dependency names without loading executable code.

EDUCATIONAL OVERVIEW:
  Untrusted offsets are bounded before reads. Results are published atomically
  after validation, and the same format cases are shared with Python consumers.

AUTHOR AND ORGANISATION:
  Sammy Hegab
  Umicom Foundation

LICENCE:
  MIT for Umicom contributions. Existing third-party terms remain applicable.
-->

# Portable Executable import inspection

Include `umicom/education/pe_imports.h` and link `Umicom::pe_imports` from
`cmake/UmicomPeImports.cmake`. The module reuses the existing binary-header
inspector. It does not change that inspector's public structure or functions.

`UmiEducationPeImportsInspectBytes` accepts a complete bounded byte span and a
caller-owned result. It checks PE32/PE32+ headers, stored section ranges, ordinary
imports and delayed imports. Descriptor traversal stops at a required terminator
and has its own count limit; a large directory containing associated lookup/name
data is not mistaken for a huge descriptor count.

The result identifies machine/address size and unique dependency basenames.
Names are compared case-insensitively for duplicates. Failure preserves the prior
result. The API never loads a DLL, resolves a filesystem search path or proves
ABI compatibility or authenticity. Runtime-loaded plug-ins that do not appear in
an import table are outside this contract.

The additive module can compose with an existing `Umicom::education_inspection`
target. Its focused reference build compiles the same canonical binary inspector
source when that target is not present. It does not replace the installed SDK or
register this module in every umbrella build automatically.

`tests/pe_imports/cases` contains declared byte fixtures, expected outcomes,
architectures and import lists. Each file is independently consumed by the C
regression runner and the Python prototype's `test_pe_corpus.py`. These files are
not executable binaries. The cases cover valid data, padding, address conversion,
truncation, unsafe names, count limits, overlapping ranges and output preservation.

The focused runtime-inspector application registers this corpus with CTest.
The separate helper-symbol test checks that private Setup Centre spellings can
coexist with foreign translation units using the original short names. It does
not run an installer or qualify every Framework consumer.

Reference: https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
