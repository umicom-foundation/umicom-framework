/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/commit.h
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
#ifndef UMICOM_SOURCE_CONTROL_COMMIT_H
#define UMICOM_SOURCE_CONTROL_COMMIT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_COMMIT_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_COMMIT_API_VERSION 1U

/**
 * Represent the source control commit snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlCommitSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char hash[128];
    char author[256];
    char email[256];
    char subject[512];
    uint64_t timestamp;
    int head;
    uint64_t revision;
} UmiSourceControlCommitSnapshot;

/**
 * Represent the source control commit registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlCommitRegistry UmiSourceControlCommitRegistry;

/**
 * Initialise source control commit registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_commit_registry_create(UmiSourceControlCommitRegistry **out_registry);
/**
 * Release or reset state held by source control commit registry so the same storage can be
 * reused safely.
 */
void umi_source_control_commit_registry_destroy(UmiSourceControlCommitRegistry *registry);
/**
 * Provide the source control commit registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_commit_registry_upsert(UmiSourceControlCommitRegistry *registry, const UmiSourceControlCommitSnapshot *item);
/**
 * Remove source control commit registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_source_control_commit_registry_remove(UmiSourceControlCommitRegistry *registry, const char *id);
/**
 * Find source control commit registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_commit_registry_find(const UmiSourceControlCommitRegistry *registry, const char *id, UmiSourceControlCommitSnapshot *out_item);
/**
 * Find source control commit registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_commit_registry_at(const UmiSourceControlCommitRegistry *registry, size_t index, UmiSourceControlCommitSnapshot *out_item);
/**
 * Return the number of records represented by source control commit registry without
 * changing their state.
 */
size_t umi_source_control_commit_registry_count(const UmiSourceControlCommitRegistry *registry);
/**
 * Provide the source control commit registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_commit_registry_revision(const UmiSourceControlCommitRegistry *registry);
/**
 * Release or reset state held by source control commit registry so the same storage can be
 * reused safely.
 */
void umi_source_control_commit_registry_clear(UmiSourceControlCommitRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_source_control_commit_snapshot_validate(const UmiSourceControlCommitSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_SOURCE_CONTROL_COMMIT_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_source_control_commit_registry_upsert_many(UmiSourceControlCommitRegistry *registry,
    const UmiSourceControlCommitSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_source_control_commit_registry_capture(const UmiSourceControlCommitRegistry *registry,
    UmiSourceControlCommitSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_source_control_commit_registry_replace_if_current(UmiSourceControlCommitRegistry *registry,
    uint64_t expected_revision, const UmiSourceControlCommitSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
