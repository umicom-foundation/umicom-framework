# Required and available environments

| Environment | Required before Core release | Evidence at FW-01 checkpoint 1 |
|---|---|---|
| Windows 11 x86-64, UCRT64, Debug | Full build, required tests, actual GTK4 | Not run in the delivery environment |
| Windows 11 x86-64, UCRT64, Release | Full build, installed SDK, packaged Notes, maintenance | Not run in the delivery environment |
| Named Linux x86-64, Debug | Core/native tests, GTK4, sanitizers where applicable | Debian 13 native focused tests; no GTK4 |
| Named Linux x86-64, Release | Core/native tests, SDK and installed GUI | Debian 13 focused tests and focused SDK only |
| Clean Windows machine | No MSYS2/developer PATH, icon launch and user-data preservation | Not run |
| Clean Linux desktop | Installed dependencies, desktop launch and recovery | Not run |

Debian 13 is a proposed named release profile, not owner-approved by this document. Current delivery tools: GCC 14.2, Clang 17, CMake 3.31.6. Exact target tool versions must be captured from qualifying hosts; previous logs do not prove today's installed version.

For Windows, preserve a PowerShell transcript of configure, build, inventory, tests and actual application actions. Use `ctest --show-only=json-v1` to capture registered names and `--output-junit` for the new execution. Archive source and package identities with those files. A transcript saying a component is enabled is not a GUI pass.

Required environment gaps block release, not the publication of a clearly scoped checkpoint. Resolve host access inside FW-01; do not switch to the next product because an environment is missing.
