# Offline application-file maintenance

Sammy Hegab · Umicom Foundation · MIT

## Ownership and scope

The maintenance API is an extension of **Umicom::setup_centre**. It consumes the
existing `suite.umi` catalogue and `umicom-install.umi` installed receipt. It does
not create an alternative database, package manifest or application-local file
collector. Windows GUI and CLI consumers call the same service.

`maintenance_plan.c` owns selection and before/after comparison;
`maintenance_codec.c` owns the closed journal grammar; `maintenance_transaction.c`
owns ordering and recovery; `maintenance_io.inc` shares the original native path
walker and file backend inside `io.c`; `maintenance_cli.c` is a presentation layer.
The GUI include shares Setup Centre's existing worker, cancellation and completion
lifetime. It never creates a competing worker subsystem.

This is an offline, trusted-local **file maintenance** profile. It does not
implement publisher authentication, signature/key management, release channels,
version ordering, data/schema migration, shortcut deletion, registration,
background services, archive pruning or a recursive uninstall. Removal is
receipt-controlled retirement into an archive, not space reclamation. Existing
NSIS and script routes are retained, not rewritten or invoked as the new engine.

The installed receipt is the authority for owned application files, not for user
business records. No operation recursively enumerates the installation to guess
what to delete. A pathname not present in the receipt remains outside maintenance
ownership. Personal data must nevertheless be backed up before a real deployment.

## Public contracts

Read `include/umicom/setup_centre/maintenance.h` before private source. A plan
copies its roots, selected catalogue and optional expected-receipt identity.
The config's strings need remain valid only during creation. The plan and its
mutable use-once flag are single-threaded. Summary/row accessors return borrowed
pointers invalid after plan destruction. Destroy accepts NULL.

The optional `expectedReceipt` binds a displayed installed list to later review.
GUI selections are never interpreted against a silently reordered receipt. The
CLI resolves removal IDs and binds the same identity. Update and Repair always
operate on the currently installed application set. Adding a previously absent
application through maintenance is not part of this edition.

Progress callbacks run synchronously on the executing thread. They must be
bounded, must not destroy the live plan, and must not re-enter a maintenance
operation. A nonzero callback return requests cancellation. The Win32 host posts
owned progress snapshots to its UI thread and retains all borrowed inputs until
the worker finishes. Public core functions make no GUI or process calls.

`UmiSetupMaintenanceApply` consumes one plan attempt, including rejected attempts.
Create a new plan rather than calling it again. A mismatched fingerprint refuses
execution. A successful result proves the observed owned file state only, not
application startup, database compatibility, publisher authority or future
immutability.

## Action rules

- **Update:** the source supplies every currently installed application with the
  same identifier and entry path. New selected files may be added only at absent,
  unowned destinations. Existing locally edited owned files block the update.
  Old owned files absent from the desired release are archived.
- **Repair:** expected receipt, hashes, sizes and owners remain unchanged. The
  source must supply those exact bytes. Missing files are restored; corrupt
  current bytes are retained before replacement. A changed source display title
  does not change the installed title during repair.
- **Remove:** the mask is bound to the installed list. Shared files remain while
  any application remains. Unmodified removed files are archived; edited removed
  files stay in place and become unowned. Damage in retained files blocks removal
  until repaired. Last-application removal archives the active receipt.

No semantic-version comparison is made. Selecting an older compatible catalogue
is an explicit file replacement, not an automatically authorised downgrade.

## Transaction order

1. Validate a trusted local installation and read-only plan.
2. Acquire the native per-installation lock and rebuild/compare the plan.
3. Check available space; exclusively create a new attempt archive.
4. Copy and hash-check every replacement into that archive; flush file data.
5. Write the before/after receipt snapshots, canonical journal and attempt marker.
6. Recheck the complete observed before state.
7. Atomically move the prepared marker into `umicom-maintenance.pending`.
8. Archive the active receipt, then each changed original before replacement.
9. Check the desired live file state; publish the desired receipt if any app remains.
10. Recheck the resulting state and move the pending marker to `committed.pending`.

No-replace moves are used for payload, receipts and markers. A failure never
turns into a recursive rollback/delete operation. The archive survives. Current
installed-receipt readers reject any existing pending marker, even a malformed
one. Older pre-maintenance tools do not have that contract and must not be used
on an unfinished installation.

A plan fingerprint is SHA-256 of its canonical journal, which includes root,
source, source-catalogue identity, action, removal mask, before/after receipt
sizes and hashes, and the sorted actual/desired row states. An attempt ID hashes
that fingerprint with an incrementing candidate number. Exclusive directory
creation, not randomness or the wall clock, establishes attempt ownership.

A no-op repair changes no payload and creates no transaction archive. The native
lock/owner metadata may be established to serialize even that attempt.

## Recovery and undo

`pending` reads the active attempt marker. API NOT_FOUND means no marker; an
unreadable or malformed marker is an error. CLI exit codes are 0 for no marker,
3 for a pending transaction, 1 for failure, and 2 for malformed invocation.
Other CLI operations use 0 for success, 1 for a failed operation and 2 for usage.

Recovery verifies the attempt identity and journal, the retained receipt bytes,
row-to-receipt membership, action constraints, backup contents and all current
reverse destinations. It performs a read-only reverse preflight before moving
files. Rows can be before, between two moves, after, or already restored. Any
other state is a conflict, not permission to overwrite a file.

Reversal proceeds in reverse row order, moves replacement bytes into `undone`,
restores originals, restores the receipt last and retires the pending marker.
A second process interruption is recoverable using the same state observations.
Staged, archived, undone and metadata files are retained.

Undo first checks a completed transaction's resulting receipt and owned files.
A newer state or local edit blocks it. It then publishes a new pending marker and
uses exactly the same reverse path. The earlier snapshot may itself be damaged,
especially before repair. Undo is not an integrity certification.

This transaction covers cooperating file-maintenance operations. It is not a
system-wide lock against running applications, hostile same-user processes or
administrators. Users must stop application writers. It cannot undo database
migrations or external business operations. Do not roll application binaries
back over incompatible data without a separately tested recovery process.

## Filesystem limits

Linux uses descriptor-relative path walking, O_NOFOLLOW, regular single-link
files, effective-user ownership checks, a nonblocking flock and
`renameat2(RENAME_NOREPLACE)`. Root directories must not be group/world writable;
known network filesystems are refused. Both parents of a moved file are flushed.
Unsupported no-replace or directory-sync operations fail instead of silently
falling back to an overwrite/copy/delete sequence.

Windows source uses the existing strict UTF-8/UTF-16 path conversion and pinned
ancestor handles. It rejects reparse points and multiply linked files, requires
a fixed local drive, uses LockFileEx, and requests MoveFileExW write-through
without COPY_ALLOWED or REPLACE_EXISTING. File contents are flushed by the
existing write/copy functions. This is not a claim of POSIX-equivalent directory
fsync semantics. That adapter and the GUI require Windows compilation and runtime
qualification; they were not executed in the delivery environment.

Process-crash tests and cancellation tests are real native Linux executions.
They do not establish sudden-power-loss guarantees, device-cache durability,
Windows filesystem semantics, ACL preservation or a usable restored application.
The current copy backend is a Windows payload installer; Linux test success does
not qualify POSIX executable-mode preservation as a Linux package manager.

Limits inherit the existing catalogue: 64 apps, 32,768 files per catalogue,
16 GiB per file, 128 GiB total selected payload, 32 MiB metadata. The plan allows
at most 65,536 union rows. These are admission limits, not a fixed memory promise.
Archive names have a maximum of 65,535 exclusive candidates for an identical
plan. The GUI bounds its display; the CLI prints every changed row.

## Build and validation

`examples/setup_maintenance` composes the existing focused Setup Centre build.
`umicomOS/tools/setup-maintenance` is a thin host entry into that same target.
The new tests exercise actual catalogue, I/O, hashing and maintenance code. The
Notes/Stock fixture payloads are inert bytes, never executed. A separate
integration uses actual Clang/LLD PE-format fixtures, native packing, inspection,
update and undo; those PE images are not run on Windows.

Run the public `review_client.c` example through the registered
`framework.setup_maintenance.public-example` test. It includes only public
headers and teaches config lifetime, receipt binding, borrowed row pointers and
one cleanup path. Filesystem fault injection exists only in test sources.

Windows acceptance includes the existing real-control construction test
extended to the fourth Setup Centre tab, Unicode installation paths, standard-user
permissions, locked executables/DLLs, cancellation, process interruption,
recovery, selective removal and actual remaining-application startup. All old
Applications/QEMU/Boot media controls must still work. Qualify supported target
filesystems before claiming release readiness.
