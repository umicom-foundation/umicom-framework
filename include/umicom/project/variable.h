/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/variable.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
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
#ifndef UMICOM_PROJECT_VARIABLE_H
#define UMICOM_PROJECT_VARIABLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_VARIABLE_CAPACITY 1024U
#define UMI_PROJECT_VARIABLE_API_VERSION 1U

/**
 * Represent the project variable snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectVariableSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char value[1024];
    char scope[64];
    int secret;
    uint64_t revision;
} UmiProjectVariableSnapshot;

/**
 * Represent the project variable registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectVariableRegistry UmiProjectVariableRegistry;

/**
 * Initialise project variable registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_variable_registry_create(UmiProjectVariableRegistry **out_registry);
/**
 * Release or reset state held by project variable registry so the same storage can be
 * reused safely.
 */
void umi_project_variable_registry_destroy(UmiProjectVariableRegistry *registry);
/**
 * Provide the project variable registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_variable_registry_upsert(UmiProjectVariableRegistry *registry, const UmiProjectVariableSnapshot *item);
/**
 * Remove project variable registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_variable_registry_remove(UmiProjectVariableRegistry *registry, const char *id);
/**
 * Find project variable registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_variable_registry_find(const UmiProjectVariableRegistry *registry, const char *id, UmiProjectVariableSnapshot *out_item);
/**
 * Find project variable registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_variable_registry_at(const UmiProjectVariableRegistry *registry, size_t index, UmiProjectVariableSnapshot *out_item);
/**
 * Return the number of records represented by project variable registry without changing
 * their state.
 */
size_t umi_project_variable_registry_count(const UmiProjectVariableRegistry *registry);
/**
 * Provide the project variable registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_variable_registry_revision(const UmiProjectVariableRegistry *registry);
/**
 * Release or reset state held by project variable registry so the same storage can be
 * reused safely.
 */
void umi_project_variable_registry_clear(UmiProjectVariableRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_project_variable_snapshot_validate(const UmiProjectVariableSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_PROJECT_VARIABLE_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_project_variable_registry_upsert_many(UmiProjectVariableRegistry *registry,
    const UmiProjectVariableSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_project_variable_registry_capture(const UmiProjectVariableRegistry *registry,
    UmiProjectVariableSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_project_variable_registry_replace_if_current(UmiProjectVariableRegistry *registry,
    uint64_t expected_revision, const UmiProjectVariableSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiProjectVariableEdit {
    UmiSnapshotEditKind kind;
    UmiProjectVariableSnapshot item;
} UmiProjectVariableEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_PROJECT_VARIABLE_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_project_variable_registry_edit_if_current(UmiProjectVariableRegistry *registry,
    uint64_t expected_revision, const UmiProjectVariableEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

/** Read up to capacity records starting at offset, at expected_revision.
 * Obtain the first revision with umi_project_variable_registry_revision and retain it for
 * every page. INVALID_STATE means the registry changed: discard the partial
 * collection and start a new observation. Offsets past the count are invalid;
 * an offset equal to the count returns an empty final page. A zero capacity
 * permits null items and returns metadata only; it does not advance the cursor.
 * All refusals leave items and out_page unchanged. Success copies accepted
 * values in registry order without changing the owner. items must have room
 * for capacity records; out_page is required. Output storage must not overlap
 * the registry, each other, or concurrently used storage. This performs no
 * allocation or I/O. Serialize all calls and mutations on the registry owner. */
UmiStatus umi_project_variable_registry_read_page(const UmiProjectVariableRegistry *registry,
    uint64_t expected_revision, size_t offset, UmiProjectVariableSnapshot *items,
    size_t capacity, UmiSnapshotPage *out_page);

#ifdef __cplusplus
}
#endif

#endif
