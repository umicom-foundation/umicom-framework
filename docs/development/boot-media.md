# Native boot-media contracts

Umicom Framework — Sammy Hegab, Umicom Foundation — MIT

## Ownership and composition

`Umicom::boot_media` owns image observations, immutable review plans, bounded byte transfer and native device adapters. `umicom-media` is its CLI. The Win32 adapter owns only presentation, input snapshots and a single worker lifecycle. The separate `umicomOS/tools/boot-media` project composes this implementation; it does not fork a copier or device scanner.

The existing Setup Centre media-stage/media-iso workflow, VM Manager, OS image builder, NSIS installer and script alternatives are unchanged.

## Contracts and lifetime

Public declarations are in `include/umicom/boot_media/media.h`. Returned plan pointers are owned by the caller and released with `UmiBootMediaPlanDestroy`. Plan getters return borrowed values. Plans are not concurrently mutable and are consumed before an apply attempt. A progress callback executes synchronously on the calling thread, must remain bounded, and must not mutate/free the plan, input files or held handles. Return nonzero to cancel.

Public structures have a fixed layout for this contract. Do not grow or reinterpret an existing binary structure silently. Future extensions need explicit compatibility design rather than a renamed copy of the same public API.

The CLI reconstructs a plan for `copy`/`write` and compares its fingerprint with the user-supplied prior review. This is not a persisted one-time authorisation token. It is not a signature or authenticated maker/checker service.

## Image profile

Maximum image: 64 GiB. Maximum target device: 256 GiB. Device list: 64 entries; Windows probes physical-disk numbers 0–255. UTF-8 paths are bounded to 4096 bytes and must be host-absolute. Windows ordinary paths are drive-qualified; UNC, alternate streams and arbitrary device namespaces are outside the regular-file profile.

The inspector accepts a bounded sector-addressable image with an MBR/GPT layout or a supported El Torito catalogue. It does not unpack compressed archives. MBR ranges are checked; non-optical plain MBR overlaps are rejected. GPT checks include primary/backup headers and CRCs, table CRCs, entry ranges, duplicate partition IDs and overlap. GPT entries are bounded to 256 and entry widths to 128–512 bytes in multiples of 128. The optical reader handles 2048-byte blocks, up to 64 descriptors and one 2048-byte catalogue sector, with no-emulation BIOS/EFI entries. It is not a universal ISO or firmware verifier.

A digest identifies observed bytes. No publisher trust, malware scan, guest-boot result, partition-filesystem consistency or authenticity decision is inferred from structure or hashing.

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
