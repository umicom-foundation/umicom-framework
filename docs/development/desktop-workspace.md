# Desktop workspace integration

Sammy Hegab · Umicom Foundation · MIT

## Responsibility

`Umicom::desktop_workspace` owns a small persistent desktop notebook and its
presentation settings. It complements the existing Desk federation, System
Centre and session handoff. It does not introduce a new OS init system, login,
compositor, automatic application restore, VM runtime or privileged service.

The public C ABI is in `umicom/desktop_workspace/workspace.h`. The GTK adapter
is a separate target, `Umicom::desktop_workspace_gtk4`; the Desktop module adds
only its launcher. No old application implementation is replaced by this batch.

## State and ownership

A caller-owned snapshot is a draft. `Commit` requires both the explicit expected
revision and the draft revision to match the object's saved revision. The
repository also compares the stored head before changing it. An object is
single-threaded; move it between workers only while idle.

`OpenServer` borrows a dedicated Data Server connection. Its caller must provide
exclusive lifetime access to the namespace and connection. `OpenMemory` owns
the canonical memory backend. `OpenDirectory` owns the canonical SQLite backend
and a cooperating-process filesystem guard held until destruction. SQLite
unavailability is returned to the caller; it is never hidden by a memory fallback.

Linux requires a private final directory owned by the current uid and refuses
symbolic-link ancestors, linked database/lock files and unsafe SQLite sidecars.
Windows rejects non-local drive paths and reparse-point traversal, holds a
non-sharing lock-file handle and rejects linked database/lock files. Its directory
ACL remains the caller's responsibility. The path is selected by the current
user, not by untrusted note content. These guards do not defend against arbitrary
malicious code running as that same user or administrative code.

## Persistent representation

`desktop.workspace.head` records the current revision, retained lower bound and
clean-close flag. Each checkpoint has a canonical binary payload with explicit
little-endian fields and byte lengths. It is stored as <=1024-byte chunks encoded
as lowercase hexadecimal, plus a bounded byte-count/chunk-count/digest record.
The schema never serialises native struct padding, pointers, paths or commands.

All persistence uses the existing Data Server. No production file in this
component calls SQLite or executes raw SQL. Test-only Data Server execute calls
install explicit failure-injection triggers.

The newest eight checkpoints are retained. Adding the new generation, retiring
the oldest known generation and updating the head form one Data Server
transaction. Cache publication happens only after successful commit. A failed
rollback poisons the object; further mutation/clean-close attempts are refused.
Hashes detect accidental storage corruption, not malicious changes accompanied
by recomputed hashes.

## Session and recovery semantics

Opening validates the current checkpoint and records an active session. An old
active marker is reported as an unfinished prior session, not a diagnosed crash.
Destroy releases resources but does not write a clean marker. CloseClean is
explicit and idempotent after successful completion.

Restore loads and checks a retained checkpoint, then commits its data as a new
revision. It is not a database-wide rollback, application transaction reversal,
OS-image recovery, arbitrary document autosave or permanent backup mechanism.
Unsaved draft text is not recoverable after a process crash.

## GTK lifecycle

No storage opens when the launcher or empty window is constructed. One GTask
per window owns storage work; the main thread disables editing until completion.
The task keeps the window alive, and the application is held during the task.
The root is retained until its state is released, so externally removed content
is not redrawn. A normal close waits for work and asks how to handle unsaved
edits. Appearance CSS is scoped to a unique class on this workspace body; it
does not alter global GtkSettings or another application's desktop preferences.

The interface and its three registered GTK lifetime checks are not claimed as
compiled or executed by the headless validation. The complete Desk build and
installed Windows/Linux workflow are separate acceptance steps.

## Limits

16 notes; identifiers up to 39 ASCII bytes; titles up to 95 UTF-8 bytes; bodies
up to 4095 UTF-8 bytes; eight retained checkpoints; fonts from 10 through 28 pt.
The complete snapshot fits within 70,000 encoded binary bytes before hex
chunking. Records stay within the canonical Data Server value and record-count
limits. Overflow, malformed UTF-8, invalid enum values, duplicate note IDs and
missing selected-note references are rejected.

## Test composition

The focused CMake project compiles actual `src/data/data_server.c` and
`src/native_launcher/sha256.c` dependency subsets, not stubs. It is not the full
Framework SDK or application build. It exports its own focused package config
so an independent consumer can check the installed ABI/link interface.

The suite covers drafts, revisions, retained history, native canonical encoding,
malformed data, 10,000 deterministic mutation inputs, real SQLite rollback
failures, cross-process locking and a child process ending without clean close.
No test formats a disk, launches an OS guest, opens a real customer document or
claims sudden-power-loss qualification.
