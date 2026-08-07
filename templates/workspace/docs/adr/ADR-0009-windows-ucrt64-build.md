# ADR-0009: Windows UCRT64 developer profile

Status: Accepted

The canonical current Windows developer profile uses PowerShell, Git, GitHub CLI, CMake, Ninja, Clang and MSYS2 UCRT64 for GTK4 and related libraries. A later self-contained release profile may remove the developer dependency on MSYS2, but UCRT64 is the quickest reproducible baseline for current Studio development.
