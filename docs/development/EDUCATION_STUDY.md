# Education study routes and guide catalogue

The existing `Umicom::education_workspace` owns catalogue semantics and learner persistence. `study.h` adds read-only copied projections; it is not a parallel learner store.

## Contracts

Capture runs on the original workspace owner thread. It copies the learner snapshot and twelve progress items. It does not copy learning notes. Immutable canonical lesson records have process lifetime. Share a capture only while its owner guarantees its lifetime; destroy only with exclusive ownership.

Search is a bounded UTF-8 byte substring with ASCII case folding. It never searches quiz answers, hints or notes. Routes contain unfinished quiz lessons from the beginning of the chosen course through the goal. All three courses remain independent. Estimated minutes come from metadata. Recovery-required snapshots can be inspected but cannot recommend a next mutation.

The 22-guide catalogue is metadata. It does not scan files, start browsers, fetch assets, run examples, grant progress or attest platform qualification. `library-html` generates the same catalogue through the existing bounded HTML writer and brand functions. The generated index uses local relative links and must be kept beside the guides.

## Storage correction

Before a learning action, `EwCommit` already replays stored events within a Data Server transaction. The new fieldwise comparison checks the resulting semantic state against the loaded state, after the existing revision check. It covers name, revision and all stored lesson progress fields, without comparing padding or derived prerequisite flags. Changed state returns BUSY and preserves the running projection. Explicit reload still accepts valid changed data. Equivalent events yielding the same state are deliberately not detected; this is not exact journal identity or cryptographic integrity.

## GUI

The shared GTK panel appends an expander. It reuses the original selector and note guard. Action callbacks carry the original panel as their user data so the existing recursive disconnection path remains authoritative. The child owns its capture via root object data; it has no owning parent pointer. Refresh and filter changes invalidate the copied route. `Search guide library` prints paths only.

GTK source and eight combined lifecycle cases require actual GTK build/display qualification. The native focused SDK does not substitute fake widgets.

## Build and evidence

Use `examples/education_study` for the focused canonical subset; all existing Education tests remain. Normal full-product composition includes `UmicomEducationStudy.cmake` from the existing Education CMake module, with include-safe test definitions. The OS entry composes the same source and introduces no OS-kernel dependency.

The future programme is recorded in `docs/planning/MAJOR_BATCH_ROADMAP.md` (editable), `.html` (public navigation) and `.json` (structured metadata). Batches 43–54 are carried forward; 55–88 are a proposed extended horizon, not previously approved delivery claims.
