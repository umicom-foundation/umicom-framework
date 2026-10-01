/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/bookmarks.h
 *
 * PURPOSE:
 *   Define reusable resource bookmarks for file, workspace and remote locations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PLATFORM_BOOKMARKS_H
#define UMICOM_PLATFORM_BOOKMARKS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_BOOKMARKS_CAPACITY 512U

/**
 * Represent the bookmark snapshot data shared with callers of this public contract.
 */
typedef struct UmiBookmarkSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char label[256];
    char group[128];
    char icon_name[128];
    int32_t order;
    uint64_t revision;
} UmiBookmarkSnapshot;

/**
 * Represent the bookmark registry data shared with callers of this public contract.
 */
typedef struct UmiBookmarkRegistry UmiBookmarkRegistry;

/**
 * Initialise platform bookmarks registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_platform_bookmarks_registry_create(UmiBookmarkRegistry **out_registry);
/**
 * Release or reset state held by platform bookmarks registry so the same storage can be
 * reused safely.
 */
void umi_platform_bookmarks_registry_destroy(UmiBookmarkRegistry *registry);
/**
 * Provide the platform bookmarks registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_platform_bookmarks_registry_upsert(UmiBookmarkRegistry *registry, const UmiBookmarkSnapshot *item);
/**
 * Remove platform bookmarks registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_platform_bookmarks_registry_remove(UmiBookmarkRegistry *registry, const char *id);
/**
 * Find platform bookmarks registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_platform_bookmarks_registry_find(const UmiBookmarkRegistry *registry, const char *id, UmiBookmarkSnapshot *out_item);
/**
 * Find platform bookmarks registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_platform_bookmarks_registry_at(const UmiBookmarkRegistry *registry, size_t index, UmiBookmarkSnapshot *out_item);
/**
 * Return the number of records represented by platform bookmarks registry without changing
 * their state.
 */
size_t umi_platform_bookmarks_registry_count(const UmiBookmarkRegistry *registry);
/**
 * Provide the platform bookmarks registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_platform_bookmarks_registry_revision(const UmiBookmarkRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_platform_bookmarks_snapshot_validate(const UmiBookmarkSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_PLATFORM_BOOKMARKS_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_platform_bookmarks_registry_upsert_many(UmiBookmarkRegistry *registry,
    const UmiBookmarkSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_platform_bookmarks_registry_capture(const UmiBookmarkRegistry *registry,
    UmiBookmarkSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_platform_bookmarks_registry_replace_if_current(UmiBookmarkRegistry *registry,
    uint64_t expected_revision, const UmiBookmarkSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
