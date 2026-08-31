# Validation Report — Umicom Framework Developer Kit 0.4.0

Validation date: 3 August 2026

## Executed successfully

- Configured the source with CMake, Ninja and Clang in portable mode.
- Compiled the Framework libraries, Framework demonstration, tests and `umicom` command-line tool.
- Passed `framework.foundation`.
- Passed `framework.scaffold`.
- Used the compiled C tool to create a new Umicom workspace.
- Used the compiled C tool to create `Umicom Studio IDE` with console, GTK4 source and web source.
- Configured and compiled the generated workspace without GTK4.
- Passed the generated Studio application test.
- Launched the generated Studio console frontend.
- Launched the generated local web server and retrieved its HTML response on `127.0.0.1:18080`.

## Validation boundary

GTK4 development headers were not installed in the Linux validation container. The GTK4 wizard and generated GTK4 frontend source are included, but the Windows/MSYS2 GTK4 build must be verified on the target Windows machine by following the manual.

The install step is a developer installation. A fully self-contained Windows installer that bundles GTK runtime DLLs remains a later packaging milestone.
