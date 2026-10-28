# Preview a small Umicom C program

The optional Framework CVM preview service compiles a borrowed source span and executes its checked arithmetic instructions. It provides one result model for command-line and future editor consumers.

Read [cvm_preview.h](../../include/umicom/toolchain/cvm_preview.h). A caller owns a workspace and a result. It passes both structure sizes so incompatible storage can be rejected before access.

The service records a phase: compiling, executing or complete. It clears the previous success flag and numeric result before a new request. Only `completed == true` together with an OK return status represents a completed preview. The detailed Umicom C diagnostic remains available when a broad Framework status is insufficient.

The service performs no file I/O and starts no process. It supports only the current Umicom C arithmetic language. Its instruction budget is not a wall-clock deadline or a security boundary for arbitrary native extensions.

## Optional build

Build and install the Umicom C SDK first. The standalone project in [examples/cvm_preview](../../examples/cvm_preview/CMakeLists.txt) finds that SDK through `CMAKE_PREFIX_PATH`. It compiles only the adapter and its example; it does not require the entire Framework build.

The example prints 42 with its built-in source. A single quoted command-line argument can supply another complete arithmetic program. Unsupported source and runtime failures produce a diagnostic.

The service is not yet connected to Studio's Run command and is not automatically added to the main Framework SDK export. Explicit optional composition keeps the compiler bootstrap independent of desktop dependencies.

The regression source at [test_cvm_preview.c](../../tests/toolchain/test_cvm_preview.c) covers successful preview, subsequent syntax failure, fuel exhaustion, arithmetic overflow, storage mismatch and null source. Execution results must be recorded for the specific compiler and source being qualified.
