/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/target.h
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
#ifndef UMICOM_PROJECT_TARGET_H
#define UMICOM_PROJECT_TARGET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_TARGET_CAPACITY 1024U
#define UMI_PROJECT_TARGET_API_VERSION 1U

/**
 * Represent the project target snapshot data shared with callers of this public contract.
 */
typedef struct UmiProjectTargetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char kind[64];
    char output_uri[1024];
    int enabled;
    int default_target;
    uint64_t revision;
} UmiProjectTargetSnapshot;

/**
 * Represent the project target registry data shared with callers of this public contract.
 */
typedef struct UmiProjectTargetRegistry UmiProjectTargetRegistry;

/**
 * Initialise project target registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_target_registry_create(UmiProjectTargetRegistry **out_registry);
/**
 * Release or reset state held by project target registry so the same storage can be reused
 * safely.
 */
void umi_project_target_registry_destroy(UmiProjectTargetRegistry *registry);
/**
 * Provide the project target registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_target_registry_upsert(UmiProjectTargetRegistry *registry, const UmiProjectTargetSnapshot *item);
/**
 * Remove project target registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_target_registry_remove(UmiProjectTargetRegistry *registry, const char *id);
/**
 * Find project target registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_target_registry_find(const UmiProjectTargetRegistry *registry, const char *id, UmiProjectTargetSnapshot *out_item);
/**
 * Find project target registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_target_registry_at(const UmiProjectTargetRegistry *registry, size_t index, UmiProjectTargetSnapshot *out_item);
/**
 * Return the number of records represented by project target registry without changing
 * their state.
 */
size_t umi_project_target_registry_count(const UmiProjectTargetRegistry *registry);
/**
 * Provide the project target registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_target_registry_revision(const UmiProjectTargetRegistry *registry);
/**
 * Release or reset state held by project target registry so the same storage can be reused
 * safely.
 */
void umi_project_target_registry_clear(UmiProjectTargetRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_project_target_snapshot_validate(const UmiProjectTargetSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_PROJECT_TARGET_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_project_target_registry_upsert_many(UmiProjectTargetRegistry *registry,
    const UmiProjectTargetSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_project_target_registry_capture(const UmiProjectTargetRegistry *registry,
    UmiProjectTargetSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_project_target_registry_replace_if_current(UmiProjectTargetRegistry *registry,
    uint64_t expected_revision, const UmiProjectTargetSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
