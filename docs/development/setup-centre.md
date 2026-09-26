# Setup Centre contracts and release boundaries

Sammy Hegab, Umicom Foundation. MIT.

## Ownership

`Umicom::setup_centre` owns offline package ingestion, selected-file integrity,
installation, media inputs and provider-neutral execution requests.
`Umicom::setup_process` adapts those requests to the existing canonical
`UmiProcessExecuteWithLifetime` service. The Windows GUI is a Framework-owned
native bootstrap, intentionally independent of GTK. CMake selects sources and
exports targets; it does not implement the new runtime installer.

The existing runtime-DLL/resource collector and NSIS installer are retained.
The packer consumes their generated ownership reports as a CLOSED DATA FORMAT;
it must never call CMake to evaluate a report supplied by an end user.

## Runtime report input

The developer list begins with `UMICOM_SETUP_INPUT\t1\n` and contains:

```
app<TAB>target-id<TAB>display title<TAB>absolute runtime report
shared<TAB>helper-id<TAB>display title<TAB>absolute runtime report
data<TAB>relative payload path<TAB>absolute source file
```

The report has exactly the five assignments emitted by StageRuntime.cmake:
`UMI_REPORT_EXECUTABLE`, `UMI_REPORT_EXECUTABLE_SHA256`, `UMI_REPORT_TARGET`,
`UMI_REPORT_PRODUCT`, `UMI_REPORT_FILES`. Each is `set(KEY [==[literal]==])`
followed by LF. Runtime-file triples are relative-path|source-path|sha256,
separated by semicolons. Trailing commands, variable expansion and alternate
CMake syntax are rejected rather than interpreted. A changed source digest is
an error. The current format intentionally does not accept control characters
in paths or titles.

`data` entries support explicit additional release files; they are not automatic
QEMU dependency discovery. A distributor must qualify native DLLs, firmware,
licences and corresponding source for an actual QEMU bundle. No QEMU binary is
redistributed by this source delivery.

## Native catalogue

`suite.umi` begins `UMICOM_SUITE<TAB>1<LF>`. Application records precede files:

```
app<TAB>id<TAB>title<TAB>relative entry executable
file<TAB>id-or-shared<TAB>decimal bytes<TAB>lowercase sha256<TAB>relative path
```

The install receipt has the same record layout, with header `UMICOM_INSTALL`.
It contains only selected applications and their files plus shared runtime.
Application entry files must exist and belong to their application. Shared
runtime is not optional. Paths reject dot segments, alternate streams, Windows
reserved names and ASCII case conflicts. Exclusive native creation also refuses
filesystem-level aliases rather than overwriting their existing target.

Limits: 64 applications, 32768 payload files, 16 GiB per file, 128 GiB total,
32 MiB catalogue text, 4095 bytes per absolute path, 767 per relative path.
Heap use is bounded but a maximum catalogue can consume hundreds of megabytes.

## Installation guarantees and limits

A review reads inputs and fingerprints the exact catalogue bytes, selection and
destination. Installation rechecks, exclusively creates a new destination and
hashes each copy. The receipt is written last. No recursive cleanup, upgrade,
repair, app launch, privileged service registration or autostart is performed.
A failed write can retain a partial file, including a partial final receipt.
Verification rejects an incomplete/malformed receipt or mismatched payload.
The process does not claim transactional crash-atomic filesystem persistence.

Directories and files are opened with no-follow/reparse checks. Windows ancestor
handles deny deletion while in use. This is not a defence against a malicious
administrator, filesystem, concurrently mutating trusted directory or compromised
publisher. Checksums and plan fingerprints are not signatures. Do not elevate
this bootstrap or give it an untrusted release. File permissions on POSIX are
private test/staging permissions; this is not a Linux application installer.

Current-user Windows shortcuts are serialised with the Shell Link COM interface
into memory, then written through the same exclusive native file writer. They
use a new group and do not overwrite an older group. Shortcut results are
separate from installation results.

## QEMU and media

The QEMU plan admits native host executables and a minimally checked kernel and
gzip header, hashes the inputs and derives explicit argument boundaries. It uses
TCG with no guest storage device, NIC, monitor or shared directory. Configuration
is reconstructed before execution; changed inputs reject the old fingerprint.
The checks do not validate a whole gzip/CPIO root filesystem, authenticate QEMU,
lock binaries until execution or establish a successful guest boot. Use trusted,
reviewed image builds and retain the existing image-builder verification.

QEMU return code zero is PROCESS SUCCESS ONLY. The existing OS foundation boot
report and service sequence need their own acceptance. The interactive console
needs a QEMU graphical backend; this is still a text-console guest, not the
completed graphical Umicom OS desktop. WHPX acceleration is not selected here.

`media-stage` writes known x86-64 inputs plus a normal/recovery GRUB menu into a
new directory. `media-iso` is Linux-only and calls an explicitly selected native
`grub-mkrescue`. It copies only the three verified tree inputs and checks ISO9660
and El Torito output structures. QEMU, GRUB, BIOS/UEFI boot and actual hardware
acceptance are independent checks. This layer has NO USB-device writer and NO
optical burner. Do not claim bootability based on synthetic header tests.

## Next release milestones

1. Windows compilation, hidden-GUI check, clean-machine all-app installation,
   cancellation, shortcut and double-click acceptance.
2. Qualified QEMU runtime bundle, exact dependency/source/notices inventory,
   real normal/recovery boots, then authenticated VM profile persistence.
3. Real BIOS/UEFI ISO tests, kernel/driver coverage and signed release metadata.
4. Separately privileged removable-device writer with device identity, system-
   disk refusal, explicit erasure confirmation, exclusive locking and read-back.
5. Optical-media provider, upgrade/repair/removal and later accelerated profiles.

A new hypervisor is not a prerequisite. A future provider can implement the
execution boundary without changing application installation logic.
