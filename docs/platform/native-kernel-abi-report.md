# Read a native Kernel ABI capture

![Umicom](../../resources/brand/umicom-logo.svg)

[Umicom application icon](../../resources/brand/umicom.ico)

An application binary interface describes how compiled programs exchange
registers, data and results. The Umicom Kernel's native `abi --record` command
prints the compiled interface inside a bounded text frame. A Framework client
can read that frame on an ordinary host without starting an emulator.

## Save only the frame

1. At the native Kernel's trusted console, enter `abi --record`.
2. Save the full frame in a new file in your evidence folder. Start at
   `UMICOM_NATIVE_ABI_REPORT 1` and end at `UMICOM_NATIVE_ABI_END`, including its
   final newline. Keep the original serial transcript and guest identity as
   separate evidence.
3. Check the fields against the linked guest. This example illustrates one
   RV64 image with ABI 1.0. Other builds may carry different compiled bits.

```text
UMICOM_NATIVE_ABI_REPORT 1
abi.version=1.0
abi.word-bits=64
abi.byte-order=1
abi.page-bytes=4096
abi.copy-limit=64
abi.call-limit=32
abi.compiled-features=31
UMICOM_NATIVE_ABI_END
```

The outer `1` is the frame's schema version. The `1.0` value is the native
interface's major and minor version. A feature bit is a yes/no observation
about a compiled route. The captured mask may include future bits that your
consumer does not understand. A live permission check is a separate operation.

## Read the capture from C23

Include `umicom/platform/native_kernel_abi_report.h` and link
`Umicom::platform`. Supply an explicit absolute path in a trusted directory.
Use a worker when the caller has an interface, since a file read can block.

```c
UmiNativeKernelAbiReport report = {0};
UmiStatus status = UmiNativeKernelAbiReportLoad(chosen_absolute_path, &report);
if (status == UMI_STATUS_OK) {
    /* Compare this observed ABI version with the caller's supported range. */
}
```

Keep the fields together when presenting an accepted snapshot. If the file
cannot be read or parsed, preserve the earlier observation and show the
refusal. `UMI_STATUS_UNAVAILABLE` identifies an unsupported frame version.
For `UMI_STATUS_PARSE_ERROR`, look for a pasted prompt, altered order, damaged
number or missing final newline. Keep only the selected frame in the input.

## Run the hosted reader checks

From a configured Framework source build with tests enabled, build the
selected target and run the ABI report cases.

```powershell
cmake --build build/framework --target umicom-native-kernel-abi-report-tests
ctest --test-dir build/framework -R '^framework[.]os_foundation[.]native_kernel_abi_report[.]' --output-on-failure
```

The hosted cases should accept a complete LF or CRLF frame, preserve unknown
bits, and reject a damaged frame without changing the output. Run the Kernel's
separate QEMU console fixture to establish whether the current guest emits
the expected record. Keep host parsing, guest execution and hardware evidence
separately attributable in the resulting test records.
