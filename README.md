# Umicom Framework

Umicom Framework is the reusable C23 application foundation for Umicom Studio IDE, Umicom Trader, Umicom Treasury Management System and future Umicom applications.

## Foundation 0.4.2

This release provides:

- status, result and memory contracts;
- diagnostics and logging;
- command, query and event buses;
- an append-only event journal;
- Data Server contracts with optional SQLite support;
- configuration, filesystem, process, workspace, document and clock services;
- module descriptors, a module registry, scheduler and Master Controller;
- a plug-in boundary;
- a project-scaffolding service;
- CMake package export through `Umicom::Framework`;
- demonstration and automated tests.

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
