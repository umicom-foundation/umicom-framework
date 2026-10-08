# Native virtual-machine manager

Umicom Framework · Sammy Hegab, Umicom Foundation · MIT

## Ownership and composition

`Umicom::vm_manager` owns profiles, runtime records, reviewed launch, QMP and managed-disk coordination. `Umicom::process_channel` owns a live child and private bidirectional standard streams. This extends rather than changes the existing synchronous process runner. `Umicom::setup_centre` exposes a small checked-file facade over its existing I/O; there is no second installer or disk writer. `Umicom::os_image` remains the native bundle authority. The Data Server remains the sole profile persistence authority.

The GUI is `umicom-vm-manager`, a native Windows application separate from the installer bootstrap. Keeping it separate avoids requiring SQLite or other application DLLs to open the installer. The CLI is `umicom-vm`. The OS host project composes the same Framework target; it has no duplicate implementation.

## Lifetimes and concurrency

Inputs are borrowed for calls; profiles are copied. Profile Save/Remove require no active caller transaction and compare the current revision inside a Data Server transaction. A visitor must not re-enter its Data Server connection. SQLite must be built for durable CLI/GUI profiles; unavailable persistence is never replaced silently by memory.

A session and a process channel have exactly one caller at a time. Calls can block and belong on a worker. Observe polls local process state; Query requests fresh QMP state. AdoptChannel transfers ownership only on successful negotiation and query. Destroy force-stops a live child; UI callers must obtain the user's decision before doing so.

Linux uses a private input socketpair, output pipes, a held native executable descriptor and fexecve, a reduced environment, an owned process group, and parent-death handling. Reap observes the root's exit without first relinquishing the PID, signals the owned group, then reaps. Windows uses a standard-handle allowlist, overlapped bounded input, suspended launch and a kill-on-close job. No transport listens on TCP. These are lifecycle boundaries, not a malicious-code sandbox. Linux PR_SET_PDEATHSIG is tied to the creating thread, so it must remain alive while the child is needed. Use a persistent worker (the Linux CLI uses its long-lived main thread), not a detached one-shot launch thread. Windows job ownership is not tied to the worker thread. See the Linux man-pages PR_SET_PDEATHSIG reference for this platform distinction.

Runtime and image hashes describe the checked bytes at review/start. Protect those input directories and do not change them during a run. The manager is not an immutable filesystem or a defence against a malicious process with the same host account.

## QMP semantics

Frames are at most 65536 bytes; nesting is at most 24 and token storage 512. The JSON reader rejects malformed UTF-8, bad surrogate pairs, duplicate decoded keys, invalid numeric primitives and trailing data. Existing language-runtime JSON interfaces remain untouched. Commands are an enum allowlist. Console bytes use QMP base64 and a 2048-byte bound.

The adapter negotiates capabilities, correlates numeric IDs and distinguishes events/errors/returns. Wrong IDs are ignored only within bounded receive and time limits. An uncertain write, timeout or malformed response disables further control; there is no reconnect or automatic action replay. Pause and Resume query state after acknowledgement. Powerdown acknowledgement is not completed shutdown. QEMU Quit/Force are not guest filesystem shutdown. Start uses -S and checks the observed initial state before exposing the session.

## Runtime package and loader boundaries

A runtime has one host label and one guest architecture, at most 1024 files and 4 GiB total. The selected native emulator and qemu-img are distinct direct children of bin. Firmware has a declared directory with at least one file. Licence/source-notice records must exist and be nonempty when packing; their text is not a legal-compliance certificate. Hash agreement is not a publisher signature.

Runtime verification checks exact inventory and hashes. Undeclared files, links/reparse points and disallowed aliases are refused. A publisher supplies the full actual inventory, then qualifies that exact build on the intended host. This batch ships no QEMU binary, firmware or operating-system image.

`runtime-component` produces bounded data records consumed by the native packer. `UMICOM_QEMU_COMPONENT_INPUT` is optional. QEMU files are owned by the `umicom-vm-manager` application, installed under share/umicom/qemu, and omitted when that application is not selected. The existing release inspector checks its private bin separately from the main application bin. The earlier global lookup remains in source; its result is superseded for this explicit private namespace.

## Managed disks

Disk creation invokes the sealed native qemu-img through the same live channel with bounded output/deadline. Only standalone QCOW2-v3 within the initial header/feature limits is accepted. No backing chain, encryption, raw device or host partition is accepted. A retained single-link lock file coordinates sessions and cold copies. Never delete that lock while any process might use it. External writers do not participate in Umicom's lock protocol.

A cold checkpoint is a new offline byte copy plus validation, not a RAM/CPU snapshot. Clean guest shutdown remains required for a filesystem-consistent source. Archives are not overwritten or automatically pruned. Full VM snapshots, acceleration qualification, guest agents, graphics embedding and networking are separate work.

## Evidence boundaries

The primary tests are C23. Codec and persistence tests use the actual library/Data Server. Native process tests use an explicitly inert protocol peer. Full Start tests use real image assembly with synthetic kernel/program headers and the inert peer; they do not boot an operating system. Packaging tests use synthetic PE metadata and the actual packer/inspector; they do not load Windows images.

`UMICOM_VM_TEST_QEMU` is empty by default. Supplying an explicitly trusted native QEMU registers an actual QMP handshake/query/quit check without a guest kernel; that still does not qualify a guest boot. The Windows controls test constructs the actual UI without opening a database or starting a VM. Neither Windows nor actual QEMU results may be inferred from Linux fixture success.

The focused SDK compiles real canonical dependency sources but is not the complete Framework or Applications build. Use the full application preset, clean-machine installer checks and real image normal/recovery checks for release qualification.

## Public lesson

See `../learning/run-umicom-in-a-virtual-machine.html` and `examples/vm_manager/profile_lesson.c`. The saved profile lesson performs no file, emulator or network action. Existing scripts, installer paths, prior APIs, licences and comments are retained.


## Standalone Kernel and Linux boot requests

The shared boot API also accepts explicitly selected Kernel firmware, direct Linux
kernels, BIOS-compatible x86 ISO media and raw disk images. See
[Boot Kernel and Linux images](../learning/boot-kernels-and-linux-with-qemu.html)
for the plan, review, paused-start and local-profile workflow. The existing sealed
runtime and packaged-image services above remain available.

Standalone boot plans copy their arguments, use the current QMP session owner and
never pass a shell command to the process layer. Selected disk images use temporary
overlays, so guest writes are not a persistent installation. These plans do not
inventory QEMU's implicit firmware or shared libraries. New GUIs should use the
public boot and profile APIs rather than construct their own argument strings.
