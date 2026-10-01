/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/staging.h
 *
 * PURPOSE:
 *   Define a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
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
#ifndef UMICOM_SOURCE_CONTROL_STAGING_H
#define UMICOM_SOURCE_CONTROL_STAGING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_STAGING_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_STAGING_API_VERSION 1U

/**
 * Represent the source control staging snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlStagingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char change_id[128];
    char hunk_id[128];
    int staged;
    int partial;
    uint64_t revision;
} UmiSourceControlStagingSnapshot;

/**
 * Represent the source control staging registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlStagingRegistry UmiSourceControlStagingRegistry;

/**
 * Initialise source control staging registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_staging_registry_create(UmiSourceControlStagingRegistry **out_registry);
/**
 * Release or reset state held by source control staging registry so the same storage can
 * be reused safely.
 */
void umi_source_control_staging_registry_destroy(UmiSourceControlStagingRegistry *registry);
/**
 * Provide the source control staging registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_staging_registry_upsert(UmiSourceControlStagingRegistry *registry, const UmiSourceControlStagingSnapshot *item);
/**
 * Remove source control staging registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_source_control_staging_registry_remove(UmiSourceControlStagingRegistry *registry, const char *id);
/**
 * Find source control staging registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_staging_registry_find(const UmiSourceControlStagingRegistry *registry, const char *id, UmiSourceControlStagingSnapshot *out_item);
/**
 * Find source control staging registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_staging_registry_at(const UmiSourceControlStagingRegistry *registry, size_t index, UmiSourceControlStagingSnapshot *out_item);
/**
 * Return the number of records represented by source control staging registry without
 * changing their state.
 */
size_t umi_source_control_staging_registry_count(const UmiSourceControlStagingRegistry *registry);
/**
 * Provide the source control staging registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_staging_registry_revision(const UmiSourceControlStagingRegistry *registry);
/**
 * Release or reset state held by source control staging registry so the same storage can
 * be reused safely.
 */
void umi_source_control_staging_registry_clear(UmiSourceControlStagingRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_source_control_staging_snapshot_validate(const UmiSourceControlStagingSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_SOURCE_CONTROL_STAGING_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_source_control_staging_registry_upsert_many(UmiSourceControlStagingRegistry *registry,
    const UmiSourceControlStagingSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_source_control_staging_registry_capture(const UmiSourceControlStagingRegistry *registry,
    UmiSourceControlStagingSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_source_control_staging_registry_replace_if_current(UmiSourceControlStagingRegistry *registry,
    uint64_t expected_revision, const UmiSourceControlStagingSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiSourceControlStagingEdit {
    UmiSnapshotEditKind kind;
    UmiSourceControlStagingSnapshot item;
} UmiSourceControlStagingEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_SOURCE_CONTROL_STAGING_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_source_control_staging_registry_edit_if_current(UmiSourceControlStagingRegistry *registry,
    uint64_t expected_revision, const UmiSourceControlStagingEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
