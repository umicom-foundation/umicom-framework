# Native OS image services

Umicom Foundation · Sammy Hegab · MIT

## Ownership and compatibility

`Umicom::os_image` is the reusable C23 host library. `umicom-os-image` is its command-line consumer. The guest's PID 1, supervisor, recovery console, configuration and Buildroot recipes stay in `umicomOS`. No code here substitutes a new guest controller or deletes the earlier `tools/os_image.py` implementation.

The native metadata formats are intentionally separate from that alternative's JSON metadata. A Python-prepared directory cannot be passed to the native `configure` or `pack` commands. Prepare a new native workspace. The public archive reader accepts canonical raw newc or this module's deterministic stored-block gzip profile; arbitrary Huffman-compressed gzip is explicitly unsupported. Standard gzip and cpio consumers can read the native output. No general-purpose extraction API is exposed.

`include/umicom/os_image/image.h` is the supported interface. Private `Oi*` names are implementation details. Returned archive bytes are allocated by the library and freed by the caller with `free`. An opened archive owns its internal copy; entry names and data are borrowed until `UmiOsImageArchiveDestroy`. No caller buffer is retained after a buffer-operation call.

## Pipeline and authority

1. `prepare` validates explicit source roots, a clean pinned Buildroot checkout and its selected native Git executable. It preflights, rereads, hashes and copies each declared source file. The final `input-manifest.umi` is the preparation completion boundary. A partial new tree remains after failure.
2. `configure` invokes an explicitly selected native GNU Make with `O`, `BR2_EXTERNAL` and `BR2_DL_DIR` arguments. Downloaded sources belong to the prepared workspace, not to a hidden shared cache. It checks the important generated options and publishes `configured.umi` with the exact `.config` digest.
3. `build` checks the captured inputs and configuration before and after the command. It verifies the selected Linux source archive with the existing Framework streaming SHA-256 API, validates the kernel configuration, checks static ELF inputs and records `built.umi` last. Buildroot's own forced package-hash checks remain enabled.
4. `pack` takes an installation-independent snapshot of the admitted program and kernel bytes. Each captured artifact must match the verified build evidence, not merely be another valid ELF at that path. It constructs the fixed diskless root filesystem, checks it with its reader, and writes `image.umi` last.
5. `verify` checks the exact bundle inventory, lengths, hashes, canonical archive, permitted modes/devices, requested architecture and generated package metadata. It cannot prove the compilation relationship, publisher identity or successful runtime execution.
6. `boot` verifies the bundle, invokes an explicitly trusted native QEMU program and evaluates the actual captured serial stream. It rechecks image and QEMU hashes after success. Result files are outside the bundle, so booting does not rewrite image metadata.

Preparing and build preflight can execute Git. `UmiOsImageReport.processLaunched` concerns the requested primary Make/QEMU operation, not these Git identity checks. Offline `assess`, archive inspection and bundle verification never launch a child. Return status is authoritative; a report describes the most recent phase and never upgrades static inspection into boot evidence.

The stored `linux-sha256` is enforced against `downloads/linux/linux-<version>.tar.xz`; the format deliberately supports the selected tar.xz kernel-source route, not every possible Buildroot kernel-source mode. A profile change requires its own compatibility and boot qualification.

## Native formats

All metadata uses LF-terminated ASCII lines and tab-separated fields. Unknown fields, duplicated fields where a scalar is required, non-canonical integers and trailing unparsed data are refused. An input file name begins with `framework/` or `umicomOS/` and contains no traversal or empty component.

`image/native/profile.umi` in the OS repository has this layout:

```text
UMICOM_OS_PROFILE<TAB>1
buildroot<TAB><40 lowercase hexadecimal characters>
linux<TAB><three decimal version components>
linux-sha256<TAB><64 lowercase hexadecimal characters>
file<TAB>framework/<relative source>
file<TAB>umicomOS/<relative source>
```

The file inventory must be lexically sorted, unique and include the profile itself. `input-manifest.umi` starts with `UMICOM_OS_INPUTS<TAB>1`, adds `arch` and records each file as `file<TAB>sha256<TAB>bytes<TAB>relative`. Its SHA-256 is the source identifier. Absolute source locations are excluded from that identifier; `location.umi` records the trusted Buildroot and Git paths separately.

`configured.umi` records source identifier and generated configuration digest. `built.umi` records source identifier, three guest executable digests, kernel image, kernel configuration and kernel source archive. Neither marker is a publisher signature. They protect against accidental stale state, not an attacker able to replace the entire local workspace and toolchain.

A bundle contains only:

```text
Image                         # RISC-V; bzImage for x86-64 instead
umicom-rootfs.cpio.gz
input-manifest.umi
image.umi
```

Its manifest always says `boot<TAB>not-run`. Runs create separate `request.umi`, `console.log` and `result.umi` files. A missing QEMU executable yields CLI exit 77 (unavailable/not run), never a pass.

## Filesystem and process contracts

New output paths are exclusive; existing files and directories are never merged, reset or recursively removed. Linux file operations walk parent directory descriptors with no-follow flags and reject non-regular input files. A persistent zero-length `.native-image.lock` uses an open-file-description lock so two cooperating calls in one process cannot concurrently change the same prepared tree. It is not a lock against arbitrary external editors or malicious same-user processes. Windows file adapters use UTF-8 conversion and non-reparse-point handles; Windows runtime behaviour requires target-platform validation.

Configure/build/legal-info require a normal Linux account. The operating-system process runner owns the child process group or Windows job. Cancellation, timeout and output-limit failure go through that owner. Source preparation and packing are synchronous and bounded but have no per-file cancellation callback in this edition.

The Linux standalone host catches SIGINT/SIGTERM through a lock-free atomic flag and requests the Framework cancellation token from a normal thread. The library installs no process-global signal handlers. Call `UmiOsImageMainWithCancellation` when embedding the command interface into a host with its own Stop button. Input pointers, the cancellation token and report storage must remain alive until return. GUI callers must run these synchronous operations on a worker and publish copied results to the UI thread.

Git/Make/QEMU paths, source recipes and the inherited environment are trusted execution inputs. Arguments are not assembled into a shell command, but Buildroot itself invokes its normal toolchain and build recipes. This is not a sandbox, an attestation service or a new hypervisor. No host block device, network share or disk is attached by the supplied boot profile.

## Limits

| Item | Limit |
|---|---|
| Captured source files | 256, at most 64 MiB combined |
| Individual image/source member | 32 MiB |
| Root archive, before gzip framing | 64 MiB |
| Archive entries | 128; the fixed guest profile uses 24 |
| Archive member name | 191 bytes plus terminator |
| Host path | 4,095 bytes plus terminator |
| Metadata | 256 KiB |
| Git/QEMU captured output | 4 MiB |
| Make captured output | 64 MiB |
| Make time budget | Four hours per invocation |
| QEMU time budget | 5–600 seconds, default 90 |
| Attempt directories per stage | 1,024 |

Capture is bounded in memory and published at command return. A sudden host termination may leave an attempt/result directory without complete logs or a result file. Such a directory is incomplete, not a pass. Durable writes are requested, but sudden-power-loss behaviour has not been qualified here. In-memory buffer duplication means peak memory can exceed the archive-size limit.

The gzip encoder uses stored DEFLATE blocks with CRC32 and length validation. This trades compression ratio for a small native implementation and no compressor dependency. It must not be advertised as efficient compression. Directory order, inode assignment, uid/gid, timestamps and padding are deterministic for identical input bytes. That property does not prove bit-reproducibility of a compiler or the complete Buildroot toolchain.

## Boot acceptance

A normal serial run must contain one init marker, successful platform-check and framework-probe lifecycles in order, one final report with the captured source identifier, `planned=2`, `completed=2`, normal/ready/none states and a shutdown marker. Recovery must contain no normal services, `planned=2`, `completed=0` and recovery/recovery/requested states. Exit zero is also required. Duplicated, misplaced, truncated or contaminated control records and kernel-panic messages reject the result.

Direct-kernel QEMU boot is not BIOS/UEFI boot, an ISO test or physical-media validation. Console assertions come from the selected trusted guest. An arbitrary native executable can imitate them; the tool is not a defence against a maliciously selected QEMU program. The report identifies its bytes so the operator can retain what was actually selected.

## Tests and examples

`framework.os_image.*` separates buffer cases from Linux filesystem/process tests. The pipeline child is explicitly named `umicom-os-image-inert-child`; it produces synthetic headers and serial text, not an OS. It is not installed. `archive_external_tools` uses GNU gzip/cpio as independent readers, not implementation dependencies. Pipeline tests create private Git fixtures and need a non-root account. An unavailable prerequisite is reported as skip code 77.

`archive_lesson.c` is a complete public-API example. It creates and reads a three-entry Notes welcome archive entirely in memory. The focused `examples/os_image` project exports a dependency-subset `UmicomImageSDK` so a separate installed consumer can be checked without pretending the whole Framework SDK was built. The normal Framework target remains `Umicom::os_image`.

### Standalone composition repair

The retained `examples/desktop_system` platform subset contained only `boot_report.c`, while its installer process adapter already required `UmiProcessExecuteWithLifetime`. The original focused Setup executable was reproduced failing to link. The delivered example keeps the entire original CMake file and appends the actual `process.c` and `cancellation.c` sources plus their installed headers to that same subset. No alternate runner or stub is introduced. This is a dependency-subset example repair, not proof that the complete Framework platform was built.

### Reference formats

Linux initramfs buffer format: https://www.kernel.org/doc/html/v6.7/driver-api/early-userspace/buffer-format.html

Buildroot manual, br2-external and out-of-tree builds: https://buildroot.org/downloads/manual/manual.html

QEMU RISC-V virt: https://www.qemu.org/docs/master/system/riscv/virt.html

No upstream source is copied into this module. The old OS build recipes, their version pins and independent recovery implementation are preserved.
