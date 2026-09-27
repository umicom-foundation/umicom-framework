# Focused risk review — reconstructed Batch 29

This is a newly reconstructed replacement, not a recovered copy of the absent earlier archive. The earlier announcement is not validation evidence. Scope: canonical Data Server, direct transaction consumers exercised by the existing Desktop Workspace suite, and the reported Windows disk-query compilation path.

## Reproduced baseline defects

1. **Cross-thread transaction participation.** A native competing thread returned OK for a memory write inside another thread's transaction. The owner's rollback removed it. The corrected implementation returns BUSY before accepting the write.
2. **False rollback completion.** A real SQLite authorizer denied ROLLBACK. The baseline reported no transaction although sqlite3_get_autocommit proved the connection still had one. The corrected implementation retains ownership/active state, blocks ordinary work and permits a later rollback retry.
3. **Binding error treated as absence.** Reducing SQLite's actual length limit caused a long key's bind to fail. The baseline get reported NOT_FOUND. The corrected get propagates CAPACITY_EXCEEDED. Set/delete binding checks are covered too.
4. **Unprotected transaction observation and shared diagnostics.** Source review found an unlocked state read and a borrowed mutable error buffer. The new observation locks consistently; legacy diagnostics use a per-thread copy and a new API offers caller-owned copying. The snapshot now captures its fields under one lock and propagates count failure.
5. **Windows SDK compatibility.** The supplied build log shows missing GET_DISK_ATTRIBUTES/IOCTL symbols. The old code remains disabled for review. The active block is compiled only when its required documented SDK symbols exist; missing support returns UNAVAILABLE, never writable. Full Windows compilation remains NOT RUN.

## Compatibility changes

- A server's transaction is bound to its initiating thread. Cross-thread handoff is no longer accepted implicitly. Coordinate work on one owner or use distinct server instances with their own transaction boundaries.
- A failed rollback with a still-active backend requires rollback recovery. Ordinary work and commit are refused.
- An automatically aborted SQLite transaction keeps its responsibility token until its owner acknowledges loss through rollback. The call returns IO_ERROR, preserving the existing Desktop Workspace's conservative stop/reopen response.
- Native worker tokens are not reused. A worker that dies in a transaction leaves the instance reserved. After all workers are joined, destroy/reopen it; no forced takeover is introduced.
- The legacy last_error result is now a thread-local copy, overwritten by the next last_error call on that thread, including one for another server. Use UmiDataServerCopyError for a longer-lived copy. Neither interface provides a per-request error history.
- Legacy count still returns zero on failure. UmiDataServerCountChecked distinguishes failures from an empty store.
- SQLite creation clears its output on failure and refuses paths that cannot be represented fully in its retained path buffer.

## Residual risks — not claimed fixed

**Object lifetime:** destroy requires all dependent workers to stop and join. Reading a pointer after destruction remains invalid. No reference-counted shutdown, owner-death handler or cancellation framework was added.

**Callbacks:** borrowed enumeration strings are valid only during the callback. Direct data API re-entry returns BUSY, but blocking on another worker, longjmp, destruction, and cyclic cross-server lock order can still break progress or lifetime. Keep visitors bounded and nonblocking.

**Spin locking and scheduling:** the original atomic-flag lock remains. SQLite operations can block, so a contending worker may spin. This package does not replace it with a native blocking mutex or establish real-time performance bounds.

**Trust:** the raw SQL entry point remains a trusted internal capability. It is not a sandbox, authentication gate or schema migration framework. Do not expose it directly to untrusted users or source documents.

**Durability:** memory storage is not persistent. Existing SQLite settings remain. Process-death and disk/power-loss qualification are distinct; this reconstruction adds no sudden-power-loss evidence.

**Physical devices:** the complete Windows path, privileged acquisition, volume exclusion, surprise removal, USB writing and boot qualification remain unverified. Keep writes OFF. On SDKs missing the attributes API, discovery can return no eligible devices by design.

**Memory evidence:** ASan/UBSan with leak detection passed the executed Linux data tests and existing Desktop Workspace tests. This is not proof of leak freedom across all applications, GUI callbacks, vendor adapters, allocation-failure branches or operating systems. No full-repository memory audit or ThreadSanitizer result is claimed.

## Preservation and ownership

The original Data Server bodies, comments, headers and APIs are retained. Superseded bodies sit in documented #if 0 blocks; new implementations explain their shared ownership rationale. Public structure layouts are unchanged. The private opaque server gains ownership/recovery state. The code remains C23; no Python feature implementation is shipped.
