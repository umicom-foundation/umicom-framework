# Umicom Framework

Umicom Framework is the reusable C23 application foundation for Umicom Studio IDE, Umicom Trader, Umicom Treasury Management System and future Umicom applications.

## Foundation 0.4.4

This release provides:

- status, result and memory contracts;
- diagnostics, logging, fan-out and a bounded retained diagnostic store;
- schema-driven typed settings with validation and persistence;
- command, query and event buses;
- an append-only event journal;
- Data Server contracts with optional SQLite support;
- configuration, filesystem, process, workspace, document and clock services;
- module descriptors, a module registry, scheduler and Master Controller;
- a plug-in boundary;
- a project-scaffolding service;
- CMake package export through `Umicom::Framework`;
- demonstration and automated tests.

### Diagnostic store

`UmiDiagnosticStore` is a toolkit-neutral bounded ring buffer.  It owns copies
of diagnostic text, preserves chronological order, tracks retained severity
counts, and reports total received and overwritten records.  It can be attached
directly to `UmiDiagnosticHub` as a normal diagnostic sink.

## Windows build

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug
```

## Author and organisation

- Author: Sammy Hegab
- Organisation: Umicom Foundation
- Licence: MIT

### Typed settings

`UmiSettings` validates string, boolean, integer and real values against an
application-owned schema.  Numeric ranges, defaults, descriptions, reset, load
and save operations are available without coupling an application to a GUI or
a database.
