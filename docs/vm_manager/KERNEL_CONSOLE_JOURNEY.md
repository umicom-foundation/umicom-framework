# Native Kernel console qualification through Umicom Framework

**Author:** Sammy Hegab, Umicom Foundation
**Licence:** MIT
**Scope:** Umicom Framework VM Manager, native `umicom` C23 CLI and independent Umicom Kernel.

## Why this belongs in Framework

The original Umicom Kernel is an independently buildable C23/Assembly research
kernel. It must not link Umicom Framework or import a Windows shell. Umicom
Framework already owns a native QMP parser, a supervised process channel,
reviewed QEMU boot plans and CLI commands. The console-journey service adds
interactive guest verification **over those existing contracts**.

Umicom OS can later use the same public service for its experimental
native-Kernel profile. Umicom Studio can call the transport-neutral protocol
with an application-owned session. Neither application owns another QMP parser.
The production Umicom OS kernel choice remains independent.

## User-visible commands

From `C:\umicom\Umicom-Applications`, after building the Framework `umicom`
executable and the standalone Kernel's normal RV64 image:

```powershell
$umicom = ".\build\windows-ucrt64-all-debug\bin\umicom.exe"

& $umicom qemu kernel-console --help

& $umicom qemu kernel-console `
    --qemu "C:\msys64\ucrt64\bin\qemu-system-riscv64.exe" `
    --directory "C:\umicom\Umicom-Applications" `
    --firmware "C:\umicom\umicom-kernel\build\riscv64-clang-debug\bin\umicom-system.elf"
```

Optional `--disk ABSOLUTE_RAW` attaches an explicitly reviewed **temporary
snapshot overlay**. The source RAW file is not modified by the VM Manager.
`--timeout-ms` accepts 100–180000, default 45000 per prompt/command. This is a
serial smoke journey and is **not** a persistence test.

The CLI uses the Framework's existing boot review fingerprint before launching
QEMU. It resumes the reviewed, paused process, waits for `umicom> ` and sends
three fixed read-only commands with byte-wise echo acknowledgement:

1. `cat /README` must show `Umicom Kernel development console`.
2. `ls /` must show `README`.
3. `mountinfo` must show `disk.mount-state=unused`.

A Kernel panic, unknown command, missing expected response, bad echo, transport
error, backwards/stalled clock, bounded output overflow or timeout fails the
journey. An uncertain command is never replayed. At the end the caller forces
**only its own** QEMU session to stop and releases it. This is explicitly **not**
a clean guest shutdown; successful console qualification does not establish
filesystem durability or persistent installation.

## C23 ownership

`include/umicom/vm_manager/console_journey.h` exposes a bounded callback
transport. `src/vm_manager/console_journey.c` owns protocol checks, output
bounds, response scoping and reporting without knowing QMP. The VM Manager
adapter in `console_journey_native.c` invokes the existing `UmiVmControl` API
and retains unconsumed bytes from QMP replies. `command_kernel_console.c` is a
thin CLI composition adapter. No new QEMU process launcher, network listener,
filesystem parser, OS service or PowerShell script is introduced.

The maximum command length is 127 printable bytes; the maximum marker length
is 127 bytes; at most eight steps are admitted. Each step uses a finite timeout
and no more than 32767 buffered output bytes. Transport callbacks are serial
and belong to the invoking thread. A session must not be destroyed concurrently
with a journey. The native adapter borrows the session, not its ownership.

## Testing

Build and run the focused Framework tests through the parent application's
existing CMake preset:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
cmake --preset windows-ucrt64-all-debug -DBUILD_TESTING=ON -DUMICOM_BUILD_NATIVE_TOOL=ON
cmake --build --preset windows-ucrt64-all-debug --target umicom umicom-kernel-console-journey-tests umicom-kernel-console-native-tests --parallel 2
ctest --preset windows-ucrt64-all-debug -R "framework[.]command[.]qemu[.](console[.]|kernel_console_help)" --output-on-failure --no-tests=error
```

The transport-neutral suite covers fragmentation, stale boot text, exact
single-byte echo, missing markers, rejected commands, Kernel failure markers,
read/write errors, timeouts, clock regression/stall, output limits, invalid
plans and refusal to retry. The native QMP test exercises a 1900-byte read
through the adapter's 2048-byte pending buffer and checks failure handling.
The CLI help test is registered separately. Actual guest acceptance requires
building and launching QEMU on the developer's machine.

For the independent FAT16 Stage/Finish persistence tests, continue using the
**existing** Kernel CTest registrations through `umicom qemu kernel-qualify`.
The console smoke command does not replace those durability fixtures.

## Repository ownership and packaging

Deliver this source under `C:\umicom\Umicom-Applications\framework\...`.
The independent `C:\umicom\umicom-kernel` tree and the
`C:\umicom\umicomOS` distribution are not edited by this batch. After the
Framework commit is qualified, the parent Applications and OS repositories
can separately advance their Framework submodule pointers. Do not replace the
OS `kernel` directory or add the independent Kernel as a Framework submodule.
