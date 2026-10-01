/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/build_node.h
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
#ifndef UMICOM_PROJECT_BUILD_NODE_H
#define UMICOM_PROJECT_BUILD_NODE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_BUILD_NODE_CAPACITY 1024U
#define UMI_PROJECT_BUILD_NODE_API_VERSION 1U

/**
 * Represent the project build node snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectBuildNodeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char target_id[128];
    char label[256];
    char kind[64];
    char depends_on[512];
    int state;
    int32_t order;
    uint64_t revision;
} UmiProjectBuildNodeSnapshot;

/**
 * Represent the project build node registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectBuildNodeRegistry UmiProjectBuildNodeRegistry;

/**
 * Initialise project build node registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_build_node_registry_create(UmiProjectBuildNodeRegistry **out_registry);
/**
 * Release or reset state held by project build node registry so the same storage can be
 * reused safely.
 */
void umi_project_build_node_registry_destroy(UmiProjectBuildNodeRegistry *registry);
/**
 * Provide the project build node registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_build_node_registry_upsert(UmiProjectBuildNodeRegistry *registry, const UmiProjectBuildNodeSnapshot *item);
/**
 * Remove project build node registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_build_node_registry_remove(UmiProjectBuildNodeRegistry *registry, const char *id);
/**
 * Find project build node registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_build_node_registry_find(const UmiProjectBuildNodeRegistry *registry, const char *id, UmiProjectBuildNodeSnapshot *out_item);
/**
 * Find project build node registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_build_node_registry_at(const UmiProjectBuildNodeRegistry *registry, size_t index, UmiProjectBuildNodeSnapshot *out_item);
/**
 * Return the number of records represented by project build node registry without changing
 * their state.
 */
size_t umi_project_build_node_registry_count(const UmiProjectBuildNodeRegistry *registry);
/**
 * Provide the project build node registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_build_node_registry_revision(const UmiProjectBuildNodeRegistry *registry);
/**
 * Release or reset state held by project build node registry so the same storage can be
 * reused safely.
 */
void umi_project_build_node_registry_clear(UmiProjectBuildNodeRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_project_build_node_snapshot_validate(const UmiProjectBuildNodeSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_PROJECT_BUILD_NODE_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_project_build_node_registry_upsert_many(UmiProjectBuildNodeRegistry *registry,
    const UmiProjectBuildNodeSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_project_build_node_registry_capture(const UmiProjectBuildNodeRegistry *registry,
    UmiProjectBuildNodeSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_project_build_node_registry_replace_if_current(UmiProjectBuildNodeRegistry *registry,
    uint64_t expected_revision, const UmiProjectBuildNodeSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
