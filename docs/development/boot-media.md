# Native boot-media contracts

![Umicom](../../resources/brand/umicom-logo.svg)

Umicom Framework — Sammy Hegab, Umicom Foundation — MIT

## Ownership and composition

`Umicom::boot_media` owns image observations, immutable review plans, bounded byte transfer and native device adapters. `umicom-media` is its CLI. The Win32 adapter owns only presentation, input snapshots and a single worker lifecycle. The separate `umicomOS/tools/boot-media` project composes this implementation; it does not fork a copier or device scanner.

<!-- Earlier ownership description retained for comparison.
The existing Setup Centre media-stage/media-iso workflow, VM Manager, OS image builder, NSIS installer and script alternatives are unchanged.
-->
The existing Setup Centre media-stage/media-iso commands remain available. Setup and Boot Media now share the independent ISO inspector; VM Manager, the OS image builder, NSIS installer and script alternatives retain their separate responsibilities.

## Contracts and lifetime

Public declarations are in `include/umicom/boot_media/media.h`. Returned plan pointers are owned by the caller and released with `UmiBootMediaPlanDestroy`. Plan getters return borrowed values. Plans are not concurrently mutable and are consumed before an apply attempt. A progress callback executes synchronously on the calling thread, must remain bounded, and must not mutate/free the plan, input files or held handles. Return nonzero to cancel.

Public structures have a fixed layout for this contract. Do not grow or reinterpret an existing binary structure silently. Future extensions need explicit compatibility design rather than a renamed copy of the same public API.

The CLI reconstructs a plan for `copy`/`write` and compares its fingerprint with the user-supplied prior review. This is not a persisted one-time authorisation token. It is not a signature or authenticated maker/checker service.

## Image profile

Maximum image: 64 GiB. Maximum target device: 256 GiB. Device list: 64 entries; Windows probes physical-disk numbers 0–255. UTF-8 paths are bounded to 4096 bytes and must be host-absolute. Windows ordinary paths are drive-qualified; UNC, alternate streams and arbitrary device namespaces are outside the regular-file profile.

<!-- The shared GPT inspector replaces the earlier media-local width description. Retain the previous contract wording for review.
The inspector accepts a bounded sector-addressable image with an MBR/GPT layout or a supported El Torito catalogue. It does not unpack compressed archives. MBR ranges are checked; non-optical plain MBR overlaps are rejected. GPT checks include primary/backup headers and CRCs, table CRCs, entry ranges, duplicate partition IDs and overlap. GPT entries are bounded to 256 and entry widths to 128–512 bytes in multiples of 128. The optical reader handles 2048-byte blocks, up to 64 descriptors and one 2048-byte catalogue sector, with no-emulation BIOS/EFI entries. It is not a universal ISO or firmware verifier.
-->
The inspector accepts a bounded sector-addressable image with an MBR/GPT layout or a supported El Torito catalogue. It does not unpack compressed archives. MBR ranges are checked; non-optical plain MBR overlaps are rejected. A protective type-0xee record must begin at LBA 1 and cover the remainder of the admitted image. GPT parsing uses the shared callback-based service: both headers, CRCs, arrays, reservations, active entry ranges, reserved fields, duplicate identifiers and overlap are checked. The bounded profile accepts up to 256 slots with entry widths 128, 256 or 512 bytes. The optical reader handles 2048-byte blocks, up to 64 descriptors and one 2048-byte catalogue sector, with no-emulation BIOS/EFI entries. These are structural observations, not universal filesystem or firmware verification.

A digest identifies observed bytes. No publisher trust, malware scan, guest-boot result, partition-filesystem consistency or authenticity decision is inferred from structure or hashing.

## Shared optical-image inspection

<!-- Earlier composition description retained; GPT parsing now also uses the shared image-inspection layer.
`Umicom::iso_image` owns the bounded ISO9660 and no-emulation El Torito parser used by both Setup and Boot Media. Its public callback contract is in `include/umicom/image_inspection/iso.h`. It has no filesystem, device, GTK or process dependency. Setup keeps its checked-file adapter; Boot Media keeps its owned media handle and existing MBR/GPT checks.
-->
The shared ISO component owns the bounded ISO9660 and no-emulation El Torito parser used by both Setup and Boot Media. Its public callback contract is in include/umicom/image_inspection/iso.h. The shared GPT component owns partition-table inspection through include/umicom/image_inspection/gpt.h. Neither parser acquires filesystem or device authority. Setup retains its checked-file adapter; Boot Media retains its owned media handle and surrounding MBR admission. See [GPT inspection](gpt-inspection.md) for the read-only contract and qualification limits.

The caller supplies a stable byte count and an exact-read callback. The parser returns zero flags for unrecognised non-ISO input. Recognised malformed records return a parse error; unsupported emulation, platforms or catalogue extensions return unavailable. Failed calls leave the caller's output unchanged. A single successful observation is not a general ISO conformance certificate.

A BIOS load must have a positive 512-byte sector count. For EFI platform 0xef, counts zero and one mean the image extends from its start to the source end. The start must still lie inside the source. This follows UEFI 2.11 section 13.3; it does not validate the referenced EFI filesystem or loader. See [UEFI 2.11](https://uefi.org/sites/default/files/resources/UEFI_Spec_Final_2.11.pdf) and [ECMA-119](https://ecma-international.org/publications-and-standards/standards/ecma-119/) for the underlying formats.

Setup requires both ISO9660 and supported El Torito flags after its producer succeeds. It compares observed file sizes between reopened reads, but the output must remain private against concurrent replacement. A refusal retains the newly created tree and image and leaves completion false. Calling Boot Media afterwards provides its broader media-layout and digest observations; it is not an independent implementation of the shared ISO format checks.

Owner qualification includes `framework.image_inspection.iso.*`, the existing `framework.boot_media.iso.*` cases and `framework.setup_centre.media.*`. The parser's memory cases exercise byte formats and errors. Setup's producer cases use synthetic callbacks and remain Linux-specific; none of these cases establishes an actual GRUB invocation, firmware boot, CD burn or USB-device result.

## File transfer

The default workflow creates a new regular file only. The parent exists beforehand. Existing files, symlinks, hard-linked source files and inappropriate handle types are refused. Output uses exclusive creation; failures retain partial files. The image is checked again before target acquisition, hashed while copied, flushed and read back through the owned handle. A second structural check after the inspection hash rejects changed layouts.

Keep inputs and parent directories private and free of concurrent mutation. Windows share modes exclude ordinary concurrent file writers. POSIX locks are cooperative, not a security boundary against an uncooperative writer. Windows parent checks do not defeat a privileged concurrent directory-rename attack. Read-back through the held OS handle is not proof of survival after power loss or of hardware-cache behaviour.

## Experimental physical backend

`UMICOM_BOOT_MEDIA_ENABLE_DEVICE_WRITES` defaults to **OFF**. It is not controlled by an environment variable or GUI preference. Leave it off in the delivered Windows configuration and ordinary public builds until qualification.

The backend accepts identified removable USB whole disks, not fixed media, arbitrary drive letters or optical drives. Require 512-byte logical sectors; 4Kn is rejected. Require exact target size for GPT; no GPT relocation or partition resizing is performed. Larger MBR targets also have up to the last 1 MiB after the image cleared and checked. The middle beyond the image remains untouched: this is not secure erase.

Linux observations use sysfs model/serial/geometry/disk sequence plus mount, swap and holder checks. Opening rechecks the device and kernel disk sequence with an owned descriptor and exclusive/cooperative locking. Mount-namespace, container, WSL-device-passthrough and real-device behaviour need separate qualification.

Windows observations use storage properties, device number/GUID, serial/model, geometry and volume extents. Unavailable/ambiguous identity, protected volumes or uncertain volume ownership fail closed. Target volumes must accept exclusive locks and dismount operations; open files, pagefiles and protected-volume locks can prevent acquisition. The backend does not elevate. It is not exclusion against an unrelated privileged raw writer. A Windows 10-or-newer storage identity capability is required by this profile; absence returns unavailable rather than a private replacement SDK declaration.

No physical-device test has been executed for this delivery. No optical burning, safe eject, privileged helper broker, secure erase, power-loss recovery or automatic rollback is claimed. Cancellation can follow a destructive partial write. The UI warns before an enabled device transfer and defaults its final confirmation to No.

## Windows compilation repairs

1. `src/desktop_system/windows.c` includes `ws2ipdef.h` before IP Helper consumes the NetIO declarations. Its modern 64-bit counters and `FreeMibTable` ownership are retained.
2. `adapters/win32/setup_centre.c` retains the complete former `Folder` helper/call in explained disabled blocks. Active `BrowseExistingFolder` avoids the Shell SDK typedef namespace collision without altering the public interface or picker behaviour.
3. `tests/setup_centre/test_setup.c` retains the former formatting statement and strip loop. A bounded leaf plus the existing `ScJoin` detects total-path overflow, and a drive-root separator survives trimming.

`tests/boot_media/test_windows_sdk.c` compiles the actual SDK types and function declarations without making the queries or opening a picker. The hidden media UI case constructs real controls and checks scrolling without discovering or writing devices. These Windows cases are included, not executed in the Linux delivery environment.

## Tests and remaining release work

All new runtime and primary regression implementations are C23. Fixtures intentionally contain synthetic MBR/GPT/ISO data, not bootable guests. The new-file workflow test executes the actual C practice-image program through the canonical Framework process runner, copies/compares its output, then proves that a second creation refuses the existing file.

Qualification still requires the exact Windows/UCRT64 build, graphical interaction, native target discovery, unplug/replug, locked/read-only/system-device refusal, real transfers to disposable media, read-back after reconnect, actual BIOS/UEFI/USB boots and power-loss testing. Do not relabel an unavailable or skipped case as a passed device test.

## Platform reference points

- MinGW-w64 `netioapi.h`: declarations behind `_WS2IPDEF_`; keep `ws2ipdef.h` before the IP Helper family.
- Microsoft `STORAGE_DEVICE_NUMBER_EX`: device GUID semantics and the conflict/no-hardware-identity flags. https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddstor/ns-ntddstor-_storage_device_number_ex
- Microsoft volume locking/dismount semantics: dismount alone is not exclusive ownership. https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ni-winioctl-fsctl_lock_volume

These references explain adapter choices. They are not evidence that the new adapter has compiled, run or been hardware-qualified.

## Preserve device identity in the native list

The Windows adapter binds each displayed row to its inventory entry with combo-box item data. It stores the inventory index plus one, reserving zero for an unbound row. Review retrieves that key, checks it against the current bounded inventory and copies the selected observation into the owned worker job.

This matters when a label is omitted because UTF-8 conversion fails or the control cannot add a string. A later visible row then has a different position from its original inventory entry. The adapter no longer treats the displayed row number as device identity. Removing or filtering an earlier row must not redirect a review to another device.

Label conversion uses the complete bounded path, model, serial and reason. It rejects unterminated fields and invalid UTF-8. A byte precision such as `%.127s` could split a multibyte character and hide an otherwise valid label; the replacement formats into bounded storage large enough for all fields before converting to UTF-16. The earlier population, lookup and enablement code remains disabled for engineering review.

Review is enabled only for an explicitly bound selection. This presentation binding does not replace device policy, identity re-observation, plan fingerprinting or final confirmation. Native discovery may still report a device that policy will refuse.

### Hidden-control acceptance checks

After a successful Windows build, run these individually and stop on an unexpected nonzero exit:

~~~powershell
& .\build\windows-ucrt64-debug\bin\umicom-media-centre.exe --check-selection
& .\build\windows-ucrt64-debug\bin\umicom-media-centre.exe --check-labels
~~~

Use the actual executable location from the configured build if it differs. The corresponding CTest names are `framework.boot_media.windows_selection` and `framework.boot_media.windows_labels`. Both construct hidden native controls and synthetic observations. They do not call device discovery, create transfer plans, open source files or write devices.

The selection case checks an omitted first label, later-row identity, deletion of an earlier row, missing and invalid keys, no selection, an excessive inventory count and a refreshed inventory. The label case checks a multibyte character across the former precision boundary, insufficient UTF-16 capacity, invalid UTF-8, unterminated fields and maximum supported field lengths. Zero exit is the expected result; these authored cases still require execution and do not establish real-device behavior.

Microsoft documents the returned row index and allocation errors for [CB_ADDSTRING](https://learn.microsoft.com/en-us/windows/win32/controls/cb-addstring) and the application-supplied association set by [CB_SETITEMDATA](https://learn.microsoft.com/en-us/windows/win32/controls/cb-setitemdata). The adapter uses that association rather than relying on visual order.

## Policy observation admission

The pure policy entry point requires canonical zero/one flags and bounded UTF-8 device labels. An unknown or nonbinary value is invalid input, not evidence of removability. The existing size, sector, protected/in-use/read-only and GPT restrictions remain in force.

Every supplied report is replaced with the current decision. Its status matches the return value, its phase is inspection, and its progress, creation, write and verification fields are zero. This prevents a reused transfer report from carrying unrelated success evidence into a read-only policy observation. Source bytes and digests are not validated by this function.

The policy-observation regression target uses synthetic records without filesystem or device calls. Run the authored framework.boot_media.policy_observation cases alongside the existing policy and transfer suites. Compiler, native adapter, hardware and boot qualification remain separate requirements.

## Diagnostic lifetime

Every public operation that accepts a report replaces it with the current
operation's diagnostic. The report status agrees with the return value, including
a missing output pointer, invalid path, malformed device observation or review
allocation refusal. Discovery, inspection and review cannot inherit an earlier
transfer's progress, output-created, write-started or verified fields.

The shared discovery boundary is in `src/boot_media/discovery.c`. Native adapters
implement the private `BmDiscover` observation seam. They retain existing
platform-specific restrictions and can return a partial inventory with a capacity
error. The public wrapper preserves that explanation and inventory while clearing
transfer-only fields. A NULL inventory is rejected before the native adapter runs.

Reports and output objects must not overlap borrowed inputs or each other. A
progress callback executes on the invoking thread; the final report is available
after return. The result does not make a caller-owned plan thread-safe and does
not establish that an image boots.

The `framework.boot_media.discovery_report.*` cases inject a synthetic adapter
into the actual public boundary. The `framework.boot_media.refusal_report.*` cases
exercise early refusals and inject failure at review allocation. Neither suite
queries or acquires a physical device. They are authored, unexecuted coverage;
qualify them alongside the existing file-copy and policy suites. The beginner
walkthrough is [Read boot-media results correctly](../learning/boot-media-diagnostics.html).


## Distinguish creation from completed acquisition

A new practice file can exist even when acquiring its usable handle fails.
Exclusive native creation happens before metadata validation, cooperative locking
on POSIX and allocation of the Framework handle wrapper. Failure in a later
step does not remove the file. The report now preserves that effect instead of
deriving outputCreated from the operation's final success status.

Read the flags independently:

| Observation | Meaning |
| --- | --- |
| outputCreated is 1 | This attempt created a regular-file practice destination |
| writeStarted is 1 | A payload write was attempted and may have made partial changes |
| verified is 1 | The completed transfer passed its scoped byte comparison |
| status is not OK | The operation did not complete successfully; inspect the other fields |

Physical disks already exist, so device transfers do not set outputCreated.
A zero creation flag does not mean a physical target was untouched; use
writeStarted and the failure phase. A verified byte copy still does not prove
firmware compatibility, operating-system boot or installer readiness.

When creation is reported but writing never started:

1. Preserve the reported practice destination for inspection.
2. Check the failure status and complete log. The file may be empty.
3. Correct permissions, resources or the native admission problem.
4. Review a new destination and a new plan before another apply attempt.
   Plans remain single-use, and no automatic rollback or retry is performed.

Ten authored native regular-file cases cover successful and existing paths,
allocation and metadata refusal after exclusive creation, missing parents,
invalid inputs, optional observation, Unicode and the retained convenience API.
They reserve unique fixture directories and retain their contents. Metadata
and allocation faults are injected after a real native create; no physical
device is acquired.

Thirteen injected-transfer cases exercise the real plan consumption, digest
and report logic with controlled transport outcomes. They include creation
followed by refusal, cancellation before and after acquisition, payload/flush/
verification failure, and cancellation at final notification. Injected progress
does not establish real-device behavior.

Run each command separately and stop on errors in a configured Framework tree:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-media-creation-effects-test umicom-media-open-observation-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.]boot_media[.](creation_effects|open_observation)[.]"
~~~

Also run the existing media refusal, policy, regular-file copy and verification
suites. These additions are authored source and regression coverage; native
execution, physical writes, optical burning and actual computer boot remain
separate qualification stages.


## Include handle release in the result

A copy can finish writing and still encounter an error when its native handle is
closed. The Framework now checks that final result before publishing a successful
inspection, transfer or file comparison. This does not replace flushing and
readback; all three boundaries answer different questions.

| Boundary | What its result establishes |
| --- | --- |
| Write | The write operation accepted the requested bytes, or reported a failure. |
| Flush | The operating system accepted the explicit flush request. Hardware guarantees still depend on the device. |
| Readback | The observed bytes and required extent agree with the reviewed image. |
| Release | Each owned native handle received one close attempt and those attempts reported success. |
| Boot | Firmware and the operating system actually started on a specified machine. This is a separate qualification exercise. |

The private transport function `BmCloseResult` consumes its wrapper on every
result. A null wrapper is harmless. On Windows it attempts all held volume
handles and the primary file handle even if an earlier close fails. It preserves
the first release error. On Linux it does not retry `close`, including after
`EINTR`: the descriptor may already have been released and reused. This Linux
policy is not a claim about every historical POSIX implementation.

The operation's first failure remains primary. For example, a write error followed
by a close error still returns the write error. If writing and verification
succeed but destination release fails, the transfer returns the release error,
keeps `verified` false and does not emit a successful finished notification.
The source is still given its close attempt. A failed close does not prove the
underlying resource was released; the wrapper must nevertheless never be reused.

1. Check the returned status and the matching report status.
2. Inspect `outputCreated` and `writeStarted` before deciding what evidence to retain.
3. Preserve the failed artifact and the reported error for diagnosis.
4. Create a fresh review before another attempt. Do not reuse a consumed plan or
   retry a numeric native handle from application code.

The historical void `BmClose` interface remains for cleanup after an already
known failure. Successful public operation boundaries use checked release.

### Qualify release outcomes

The tests below inject close results. They do not open or write a physical
device. The native-close seam uses synthetic handles; it checks attempt counts,
first-error precedence and wrapper consumption, not operating-system behaviour.
Linux registers six cases; Windows registers nine, including three held-volume
positions and multiple failures. The transfer suite adds five close scenarios
to its existing thirteen cases. Eight read-only cases cover inspection,
source-first comparison and failure publication.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-media-close-observation-test umicom-media-read-close-test umicom-media-creation-effects-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "framework[.]boot_media[.](close_observation|read_close|creation_effects)[.]"
~~~

Run the configured platform's normal build and metadata checks as well. These
cases are authored coverage until executed against an identified source revision.
They do not qualify firmware, optical burning, device removal or power-loss
durability.

References: [Linux close](https://man7.org/linux/man-pages/man2/close.2.html) and
[Windows CloseHandle](https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle).


## Present final reports through the shared formatter

`UmiBootMediaReportDescribe` is a pure Framework presentation service. The native
CLI and existing Windows output control use it to show status, phase, observed
byte progress, regular-file creation, attempted payload writing and byte
verification. It performs no file, device or network operation.

~~~c
char text[UMI_BOOT_MEDIA_REPORT_TEXT];
UmiStatus description = UmiBootMediaReportDescribe(operation_status,
    &report, text, sizeof text);
if (description == UMI_STATUS_OK) {
    /* Present text in the caller's existing output surface. */
} else {
    /* Preserve operation_status and report an unavailable description.
       Never display an older successful description as this result. */
}
~~~

The return value describes formatting, while operation_status describes the
media operation. The two are separate decisions. The formatter requires matching
operation/report status, binary flags, a known phase, bounded UTF-8 detail and
progress that does not exceed its total. A failed operation cannot claim
verified. It publishes only a complete string; invalid input or insufficient
capacity leaves the output buffer unchanged.

The Windows interface retains its layout, selected-device identity controls,
canonical branding and review confirmation. Successful review appends its
destination and tokens after the shared report. Successful inspection appends
structural observations. A malformed report cannot adopt a review plan or show
verified success. The CLI retains exit codes 0 for success, 77 for unavailable
capability and 1 for operation failure; argument usage remains 2. Its messages
are human-readable, not a versioned machine protocol.

Twenty-four pure cases cover effects, Unicode, maximum fields, exact capacity
and refused publication. Seven CLI seam cases cover output and early refusals
without acquiring files or devices. A hidden Windows control check presents
synthetic creation, verification and inconsistent-status reports.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-media-report-description-test umicom-media-cli-report-test umicom-media-centre
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "framework[.]boot_media[.](report_description|cli_report)[.]|framework[.]boot_media[.]windows_reports$"
~~~

These commands remain owner-run. Source review and authored hidden checks do not
establish displayed typography, screen-reader behaviour, worker shutdown or
device/firmware qualification.


## Keep the Windows worker alive independently of notifications

The existing native adapter owns one worker and one captured job. Its message
loop now waits on both the worker's native handle and the Windows message queue.
The worker's completed handle is the authority to read final fields and dispose
of the job. A posted completion message is only a wake hint: failure to deliver
it cannot strand a completed operation.

The Framework task and queue services remain the reusable choice for general
background scheduling. This adapter already owns a native waitable thread that
must participate directly in a Win32 message wait. The repair retains that
composition and the shared media service; it adds no application-specific copier,
device scanner or replacement scheduler.

- Progress lives in an atomic field of the owned job. A notification reads the
  current field rather than trusting an old queued percentage.
- Duplicate percentages do not enqueue another progress notification. A new job
  resets the progress control before launch.
- An early or stale completion message cannot block the UI waiting for a running
  worker. The adapter checks the handle without waiting before reading results.
- Close requests stop new admission and request cooperative cancellation.
- If a close notification re-enters admission before thread creation, the
  never-started job is discarded without starting filesystem work.
- Quit, destroyed-window and abnormal message-wait paths drain the worker before
  freeing its owner or captured inputs. They do not publish into a retired UI.

Cancellation remains cooperative. A native filesystem or device call can delay
the worker's return, so final draining can wait for that call. No thread is
forcibly terminated. If a native join result is uncertain, the adapter returns
failure while retaining owner state instead of freeing memory that a worker may
still use. This exceptional retention requires investigation; it is not a normal
resource-release success. A failed worker-handle release also makes application
shutdown report failure while preserving the separately recorded media outcome.

### Qualify the native lifecycle

Thirteen Windows-only cases use actual hidden controls and native threads with a
synthetic inspection service. They cover ordinary completion, dropped completion
and progress messages, early completion, stale progress, close during work, quit
during work, destroyed windows, wait failure, idle-loop error, uncertain drain,
thread-start refusal, close during admission and handle-release reporting.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-media-worker-windows-test umicom-media-centre
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "framework[.]boot_media[.]windows_worker[.]"
~~~

The test keeps a separate native thread reference and checks that the job is
freed only after that thread stops. Error seams make otherwise uncommon paths
deterministic. No image is opened, no physical device is discovered and no window
is shown by these tests. They remain authored coverage until owner execution.
Visible interface behaviour, accessibility and real I/O cancellation latency
need their own qualification.

References: [Windows message and handle waits](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-msgwaitformultipleobjectsex),
[posted messages](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-postmessagew)
and [single-object waits](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject).
