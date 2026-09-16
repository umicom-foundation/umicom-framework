# Operational state: project settings, process results and financial records

## Saving a project's settings

A Notes project should open with the build directory and Run executable you last selected. Those values belong to the project settings, not to Studio's executable defaults. Framework now stores an existing `UmiBuildProfile` through `UmiBuildProfileStoreSave` and reads it through `UmiBuildProfileStoreLoad`.

Include `umicom/build/profile_store.h` and link `Umicom::build`. The build target carries its Data Server dependency through the installed SDK. No application should call SQLite directly for these settings.

The caller first loads the current revision, edits a private profile, then saves with that revision. Revision zero creates an absent record. A later save succeeds only when the revision still matches. Another Studio instance cannot silently overwrite settings that were changed after it opened the project.

The store uses 17 Data Server values for each profile: eleven text fields, four numeric settings, a revision and a schema marker. The fields are written in one transaction. They are not a binary copy of the C structure. The schema marker is currently `1`.

All existing profile fields are retained, including the compiler, preset, build target, Run program and argument, installation prefix, parallel-job count and timeout. An empty Run program remains empty after saving. It is not replaced with Studio's executable.

### Ownership and errors

The Data Server is borrowed. Use its connection on its owning thread and do not call this API inside an already active transaction. That condition returns `UMI_STATUS_BUSY`. Separate SQLite connections use the Data Server's existing immediate transaction path. A SQLite lock error is reported rather than hidden behind an automatic overwrite or retry.

On a successful load, both output arguments are assigned. An absent record returns `UMI_STATUS_NOT_FOUND` and sets the revision output to zero without changing the profile output. Other load failures preserve both outputs. A failed save preserves the output revision and rolls back its writes; existing malformed or incompatible records are not replaced.

The workspace key uses an absolute, lexically normalised path and a platform namespace. The full workspace root is checked as well as the fingerprint. Symlink aliases are not resolved. Windows and POSIX settings are separate. This is local user configuration, not a portable project file distributed by Git.

The saved profile does not include workspace trust, permissions or authentication. Restoring a Run path does not authorise running it. Run arguments and paths are stored as ordinary configuration; do not put tokens or passwords in them.

### Studio's connection

`UmiStudioBuildProfilesBind` borrows an existing Data Server and restores the current workspace's settings when present. `UmiStudioBuildProfileSave` checks that the selected workspace matches and no build is running before calling Framework. A stale save leaves the active profile and trust choice unchanged.

The GTK host reuses the existing per-user workspace SQLite Data Server. The project form and settings form save through this binding. The host detaches it before releasing its storage connection. Headless services may instead use memory storage or an explicitly supplied SQLite Data Server. Memory storage does not survive destruction.

`UmiStudioBuildProfilesDetach` performs no storage operations. After detaching, reopen the workspace or bind another server before saving again so that a fresh revision is known.

## Releasing completed process jobs

A supervised process retains its result after it exits. This is useful until its owner has read the result, but a launcher must eventually release completed work. Otherwise a 64-slot supervisor can launch only 64 applications during its entire lifetime.

`UmiProcessSupervisorReleaseJob` releases one terminal job after the owner has acknowledged it. Created/running jobs or jobs with active waiters return `UMI_STATUS_BUSY`. Jobs are not released automatically for other clients. Release is rejected during supervisor shutdown.

The slot may be reused; the job identifier is never reused. A released identifier returns `UMI_STATUS_NOT_FOUND` on later lookups. Existing cumulative submitted, succeeded, failed, cancelled and timed-out counters remain. `stats.jobs` counts retained jobs, not the lifetime total. Snapshot enumeration lists the currently occupied slots; its order can change after release.

Live jobs stay at stable addresses. A worker's copied arguments still point into its own slot. Concurrent waiters pin that slot and join a completed worker only once. Shutdown cancels all remaining jobs and drains them; destruction still requires exclusive lifecycle ownership.

### Live output is preserved

`UmiProcessExecuteObserved` is an additive form of the existing executor. It calls a supplied observer after a complete output chunk has been appended. The observer receives a borrowed result valid only during the callback, on the executing thread. It must copy anything it retains and must not manipulate GTK widgets.

The supervisor executes into a private worker result and copies live output under the same mutex used by snapshot readers. Completion is published under that mutex too. This avoids concurrent writes to a buffer being read by another thread without removing live output. The old `umi_process_execute` API remains as the no-observer entry point, with the same request layout and validation.

The POSIX pipe reader also checks the returned byte count against the buffer it supplied to `read`. This makes that capture boundary visible to the optimising compiler. Tail retention and the existing truncated-output indication remain unchanged.

## Matching an application exit to its process

`UmiDeskRuntimeReconcileProcessExit` accepts the application identity and the process token. It rejects an old token when a new instance is already associated with the same application. An exit notification from an earlier process therefore cannot mark the new process as stopped.

The existing application-ID-only exit API remains available for compatible callers. Desk's process-owner path uses the token-aware form, records the exit, then releases the completed supervisor job. Invalid or overlong exit text is checked before changing the runtime catalogue. A rejected notification does not change the entry or its revision.

## Shared banking records and Trader drafts

The historical `umi_financial_id_is_valid` entry point now delegates to the existing bounded identifier validator. `umi_financial_id_compare` remains a lexical ordering operation, but bounds its reads to the actual arrays. Equal comparison is not proof that an identifier is valid.

A deposit account validates its own ID, customer and product IDs, currency, state and balances. Negative balances are still permitted by this record contract; the change does not invent an overdraft policy. The difference between ledger and available balance must fit `int64_t`.

`UmiBankingDepositAccountReserved` reports an error without changing its output when the account or calculation is invalid. The older value-returning helper is retained and returns zero on invalid input; consumers that need to distinguish invalid input from a true zero should use the checked form.

The deposit-account, hold and deposit-transaction initialisers build a private candidate. Failed construction leaves the caller's previous record unchanged. Account lookup checks the book count before scanning and skips malformed identifiers.

Trader's draft quantity, price and bar-update entry points reject non-finite numbers before changing the draft, its risk preview, revisions or history. These checks do not replace final order validation, pretrade risk or live-order arming. The existing pretrade market-order reference-price fallback of `1.0` remains a separate unresolved valuation limitation.

## Regression tests

The tests live in `tests/operational_state` and link the real Framework libraries. Studio's tests use real services and the Data Server. Desk's lifecycle test launches a deliberately inert child executable, not a bank, broker, shell command or customer application.

The new CTest names begin with `framework.operational_state.`, `studio.project_settings.` and `desktop.launch_lifecycle`. Their checks remain active in Release builds. SQLite-dependent tests use skip code 77 when SQLite is not built; a skipped test is not a durable-storage pass.

The directory test covers zero, one and two entries. It protects the shared walker from calling `qsort` with a null list for an empty folder while retaining normal sorting for non-empty directories.
