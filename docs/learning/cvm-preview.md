# Preview a small Umicom C program

The optional Framework CVM preview service compiles a borrowed source span and executes its checked arithmetic instructions. It provides one result model for command-line and future editor consumers.

Read [cvm_preview.h](../../include/umicom/toolchain/cvm_preview.h). A caller owns a workspace and a result. It passes both structure sizes so incompatible storage can be rejected before access.

The service records a phase: compiling, executing or complete. It clears the previous success flag and numeric result before a new request. Only `completed == true` together with an OK return status represents a completed preview. The detailed Umicom C diagnostic remains available when a broad Framework status is insufficient.

The service performs no file I/O and starts no process. <!-- Earlier profile wording: It supports only the current Umicom C arithmetic language. -->
It supports the bounded integer expression profile supplied by the selected Umicom C SDK, including comparisons when that SDK has the extension. Its instruction budget is not a wall-clock deadline or a security boundary for arbitrary native extensions.

## Optional build

Build and install the Umicom C SDK first. The standalone project in [examples/cvm_preview](../../examples/cvm_preview/CMakeLists.txt) finds that SDK through `CMAKE_PREFIX_PATH`. It compiles only the adapter and its example; it does not require the entire Framework build.

The example prints 42 with its built-in source. A single quoted command-line argument can supply another complete arithmetic program. Unsupported source and runtime failures produce a diagnostic.

The service is not yet connected to Studio's Run command and is not automatically added to the main Framework SDK export. Explicit optional composition keeps the compiler bootstrap independent of desktop dependencies.

The regression source at [test_cvm_preview.c](../../tests/toolchain/test_cvm_preview.c) covers successful preview, subsequent syntax failure, fuel exhaustion, arithmetic overflow, storage mismatch and null source. Execution results must be recorded for the specific compiler and source being qualified.


## Add a trace without duplicating the engine

Include [cvm_trace.h](../../include/umicom/toolchain/cvm_trace.h) and allocate a UmiCvmPreviewTrace beside the existing workspace and result. Pass all three sizes to umi_cvm_preview_traced. Wrong storage sizes leave the objects untouched. Once storage is accepted, the adapter clears the previous result and trace count before compiling.

A syntax error leaves the compiling phase with zero trace entries. Runtime failures leave the executing phase and retain the attempted steps. Only an OK status and completed true permit a completed-result display. Read only entries below trace.count; old entries beyond it are not part of the new request. The adapter delegates to Umicom C and uses the same state lifecycle as the original preview API.

Build the updated Umicom C SDK first, then qualify this separate consumer. The following PowerShell commands use C:\umicom; replace that root consistently if needed. Run one line at a time and stop on errors.

```powershell
Set-Location "C:\umicom\umicom-c"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --preset windows-ucrt64-debug -DBUILD_TESTING=ON
cmake --build --preset windows-ucrt64-debug --parallel 2
ctest --preset windows-ucrt64-debug --output-on-failure --no-tests=error
cmake --install build/windows-ucrt64-debug --prefix C:/umicom/_local/install/umicom-c
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake -S examples/cvm_preview -B build/cvm-preview -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_PREFIX_PATH=C:/umicom/_local/install/umicom-c -DBUILD_TESTING=ON
cmake --build build/cvm-preview --parallel 2
ctest --test-dir build/cvm-preview --output-on-failure --no-tests=error
```

The trace regression cases cover ordinary-result parity, stale success after malformed input, arithmetic and fuel failures, independent storage, shorter requests and invalid storage sizes. They do not exercise a Studio UI or a Kernel guest. A missing trace header or unresolved umc_execute_trace symbol indicates an older or mismatched SDK; correct that dependency before interpreting runtime outcomes.

## Display the trace in the example command

After building the optional example, launch `build/cvm-preview/umicom-cvm-preview.exe --trace` on Windows. The built-in program should print four successful rows followed by 42. Supply a complete source program as the next quoted argument to trace another supported expression. The earlier no-option command still prints only its result. Syntax failures publish no rows, while runtime arithmetic failures retain the rejected row and exit with a diagnostic.

The optional OS wrapper composes this same example and its checks rather than copying the command implementation. Six process cases compare exact ordinary and trace output, explicit source, arithmetic failure, syntax failure and extra-argument rejection. They complement the adapter unit cases; a native process pass does not establish OS guest startup.

## Compare integers through the same service

After installing the comparison-capable Umicom C SDK and rebuilding the optional example, run:

```powershell
& ".\build\cvm-preview\umicom-cvm-preview.exe" --trace "int main(void) { return 2 + 3 < 2 * 3; }"
& ".\build\cvm-preview\umicom-cvm-preview.exe" "int main(void) { return 2 < 1; }"
ctest --test-dir build/cvm-preview -R "cvm_comparison" --output-on-failure --no-tests=error
```

The first command should print eight trace rows and then 1. The second should print 0 and exit successfully: a false comparison is a valid completed result. Check the status and completed flag; never use the numeric answer itself as a success flag. The comparison checks also cover a subsequent syntax failure clearing previous output, eager operand failure, exhaustion before LESS or RETURN, ordinary/traced parity, signed extremes and a shorter replacement trace.

This adapter still owns no parser or interpreter. Both API entry points call the installed Umicom C implementation, and the optional OS example composes the same Framework project. The comparison test source deliberately refuses SDK headers that lack `UMC_COMPARISON_PROFILE`. Correct the SDK selection and rebuild if that check fails; do not remove it to make an old dependency appear qualified. A matching header alone is insufficient because the linked library can also be stale. These checks describe authored coverage, not executed GUI or guest evidence.
