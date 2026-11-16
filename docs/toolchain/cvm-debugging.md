![Umicom](../../resources/brand/umicom-logo.svg)

# Share a paused Umicom C session

The optional CVM adapter delegates compilation and execution to the Umicom C SDK. It owns presentation state so an editor or OS tool cannot accidentally show a previous result after a failed request.

Include **umicom/toolchain/cvm_debug.h**. Allocate one **UmiCvmDebugSession** per concurrent request. Source storage and session storage must be valid and disjoint. The caller owns the session; no background thread or GUI callback is started.

1. Call **umi_cvm_debug_start** with the source span, instruction budget, session address and its exact sizeof value.
2. Check the returned UmcStatus. Successful admission produces PAUSED with zero executed instructions.
3. Call **umi_cvm_debug_step** when the user requests an operation. Read the status and phase before displaying values.
4. A successful RETURN changes the phase to COMPLETE and publishes the result.
5. An execution error changes the phase to FAILED, retaining its source location.
6. Call **umi_cvm_debug_cancel** on a PAUSED session to enter CANCELLED. No completed result is published.
7. Start again for a new program. Accepted session storage is cleared before source admission, so failed compilation cannot expose a previous successful result.

The source can be released after start. The service retains line/column coordinates, not the source file itself. Fields are inspectable but must not be changed directly. This is an integer VM debugger, not a native C23 debugger, Java runtime or Studio breakpoint UI.

## Configure the small consumer

The default uses an installed Umicom C package. A development build may explicitly select the canonical compiler source. This avoids an implicit sibling checkout and does not require the full Framework desktop dependency graph.

~~~powershell
Set-Location "C:/umicom/Umicom-Applications/framework"
cmake -S examples/cvm_preview -B build/cvm-preview-debug -G Ninja -DCMAKE_C_COMPILER=gcc -DUMICOM_C_PREVIEW_SOURCE=C:/umicom/umicom-c -DBUILD_TESTING=ON
cmake --build build/cvm-preview-debug --parallel 2
ctest --test-dir build/cvm-preview-debug --output-on-failure --no-tests=error
& "./build/cvm-preview-debug/umicom-cvm-preview.exe" --steps "int main(void){return 6*7;}"
~~~

Run commands separately and stop on errors. The final command prints operation locations and the value 42. These are qualification instructions, not a claim that this checkout has passed them.

For an installed package, omit UMICOM_C_PREVIEW_SOURCE and set CMAKE_PREFIX_PATH to the SDK prefix. Headers and libraries must both contain the resumable machine API. An older package with the same package version can lack this new API; do not mix old libraries and new headers. The source-mode options are local to the adapter function and do not overwrite the caller's compiler-tools/test cache settings.

The OS tools/cvm-preview project reuses this consumer and service. It adds no parser or executor. Native GUI wiring, breakpoints, live source replacement and application cancellation policies require separate integration.
