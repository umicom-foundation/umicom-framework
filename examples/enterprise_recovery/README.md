# Enterprise import recovery laboratory

This C23 example runs the canonical enterprise service and Data Server. It does not open a broker, run a network connector, edit a real dataset or invent a replacement database backend.

Start with `umicom-enterprise-recovery --self-test`, then run `demo` to see the original and newly evaluated proposals. The input is a fictional workshop stock list. The saved value changes from 12 to 15 before an approved 12-to-20 import is applied. The original job is refused. A new job evaluates 15-to-20, requires a new review and links back to the original in the audit. A filtered result survives closing the memory workspace because it owns its copied rows.

The public guide is `docs/learning/ENTERPRISE_RECOVERY.html`. The contracts are documented in `docs/development/ENTERPRISE_RECOVERY.md` and `include/umicom/enterprise_workspace/recovery.h`.

## Build in a separate directory

Use CMake 3.24 or later with a C23-capable compiler and Ninja. This standalone laboratory defaults to the real SQLite backend. Set `UMICOM_RECOVERY_SQLITE=OFF` only for a deliberate memory-only comparison; applicable SQLite tests return 77 rather than pretending to pass.

```text
cmake -S framework/examples/enterprise_recovery -B build/enterprise-recovery -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/enterprise-recovery --parallel 2
ctest --test-dir build/enterprise-recovery --parallel 2 --no-tests=error --output-on-failure
```

This subset does not build a graphical application. The existing full Applications preset composes the GTK adapter. The separate `client` project consumes an installed SDK with `find_package(UmicomFramework CONFIG REQUIRED)` and uses public headers only. Install the laboratory SDK into its own practice prefix, not over an existing SDK installation.

The example uses explicitly labelled practice identities. They do not authenticate a user. Every workspace has one owner thread; borrowed services outlive it, and copied query/recovery objects have their own explicit destroy functions.
