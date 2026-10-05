/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/problem.h
 *
 * PURPOSE:
 *   Define an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_UI_PROBLEM_H
#define UMICOM_UI_PROBLEM_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PROBLEM_CAPACITY 4096U
#define UMI_UI_PROBLEM_API_VERSION 1U

/**
 * Represent the ui problem snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiProblemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char source[128];
    char code[128];
    char message[1024];
    char uri[1024];
    uint32_t line;
    uint32_t column;
    int severity;
    int resolved;
    uint64_t revision;
} UmiUiProblemSnapshot;

/**
 * Represent the ui problem registry data shared with callers of this public contract.
 */
typedef struct UmiUiProblemRegistry UmiUiProblemRegistry;

/**
 * Initialise ui problem registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_problem_registry_create(UmiUiProblemRegistry **out_registry);
/**
 * Release or reset state held by ui problem registry so the same storage can be reused
 * safely.
 */
void umi_ui_problem_registry_destroy(UmiUiProblemRegistry *registry);
/**
 * Provide the ui problem registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_problem_registry_upsert(UmiUiProblemRegistry *registry, const UmiUiProblemSnapshot *item);
/**
 * Remove ui problem registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_problem_registry_remove(UmiUiProblemRegistry *registry, const char *id);
/**
 * Find ui problem registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_problem_registry_find(const UmiUiProblemRegistry *registry, const char *id, UmiUiProblemSnapshot *out_item);
/**
 * Find ui problem registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_problem_registry_at(const UmiUiProblemRegistry *registry, size_t index, UmiUiProblemSnapshot *out_item);
/**
 * Return the number of records represented by ui problem registry without changing their
 * state.
 */
size_t umi_ui_problem_registry_count(const UmiUiProblemRegistry *registry);
/**
 * Provide the ui problem registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_problem_registry_revision(const UmiUiProblemRegistry *registry);
/**
 * Release or reset state held by ui problem registry so the same storage can be reused
 * safely.
 */
void umi_ui_problem_registry_clear(UmiUiProblemRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_problem_snapshot_validate(const UmiUiProblemSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_PROBLEM_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_problem_registry_upsert_many(UmiUiProblemRegistry *registry,
    const UmiUiProblemSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_ui_problem_registry_capture(const UmiUiProblemRegistry *registry,
    UmiUiProblemSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

/** Replace the complete collection only while expected_revision still matches
 * this registry. Use a revision from capture, not a different registry. A stale
 * proposal returns INVALID_STATE without changing records. IDs must be unique;
 * an empty proposal clears the collection, except an already-empty collection
 * needs no change. Successful publication advances the revision once and gives
 * every stored row that revision. Revision exhaustion returns CAPACITY_EXCEEDED.
 * Existing upsert normalisation remains in effect. Any validation/allocation
 * failure retains the complete previous collection. Inputs and optional result
 * must not overlap each other or registry storage; serialize owner access.
 * This is an in-memory publication, not a thread lock or a durable disk save. */
UmiStatus umi_ui_problem_registry_replace_if_current(UmiUiProblemRegistry *registry,
    uint64_t expected_revision, const UmiUiProblemSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiUiProblemEdit {
    UmiSnapshotEditKind kind;
    UmiUiProblemSnapshot item;
} UmiUiProblemEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_UI_PROBLEM_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_ui_problem_registry_edit_if_current(UmiUiProblemRegistry *registry,
    uint64_t expected_revision, const UmiUiProblemEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

/** Read up to capacity records starting at offset, at expected_revision.
 * Obtain the first revision with umi_ui_problem_registry_revision and retain it for
 * every page. INVALID_STATE means the registry changed: discard the partial
 * collection and start a new observation. Offsets past the count are invalid;
 * an offset equal to the count returns an empty final page. A zero capacity
 * permits null items and returns metadata only; it does not advance the cursor.
 * All refusals leave items and out_page unchanged. Success copies accepted
 * values in registry order without changing the owner. items must have room
 * for capacity records; out_page is required. Output storage must not overlap
 * the registry, each other, or concurrently used storage. This performs no
 * allocation or I/O. Serialize all calls and mutations on the registry owner. */
UmiStatus umi_ui_problem_registry_read_page(const UmiUiProblemRegistry *registry,
    uint64_t expected_revision, size_t offset, UmiUiProblemSnapshot *items,
    size_t capacity, UmiSnapshotPage *out_page);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_ui_problem_snapshot_archive_encode(const UmiUiProblemSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_problem_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiUiProblemSnapshot *value);

/** Encode the ordered collection only at expected_revision. Use the same
 * revision for measurement and encoding; an intervening edit is INVALID_STATE.
 * Short output reports the required size without writing bytes. The common
 * archive byte limit applies. Serialize access on the owning thread. */
UmiStatus umi_ui_problem_registry_archive_encode(const UmiUiProblemRegistry *registry,
    uint64_t expected_revision, void *bytes, size_t capacity, size_t *out_size);
/** Restore all rows only at the caller's current observation revision.
 * Every row is decoded before the existing replace_if_current operation
 * validates duplicate identities and publishes the collection atomically.
 * Corrupt, incompatible, oversized or stale input leaves the owner unchanged.
 * Publication assigns fresh local revisions; saved row counters grant no
 * authority. An empty archive deliberately clears the collection. This uses
 * bounded staging allocation plus the replacement owner's scratch storage.
 * Optional out_result is initialized on every return; rejected_index identifies
 * a bad row when available. Its validation detail describes domain publication,
 * while the return status describes envelope/codec failures. Keep input,
 * result and registry storage separate. No files, callbacks or commands run. */
UmiStatus umi_ui_problem_registry_archive_restore(UmiUiProblemRegistry *registry,
    uint64_t expected_revision, const void *bytes, size_t byte_count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
