/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/render_tree.h
 *
 * PURPOSE:
 *   Define incremental render-tree snapshots for efficient multi-frontend updates.
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
#ifndef UMICOM_FRONTEND_RENDER_TREE_H
#define UMICOM_FRONTEND_RENDER_TREE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_RENDER_TREE_CAPACITY 4096U

/**
 * Represent the frontend render node snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendRenderNodeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char widget_id[128];
    char parent_id[128];
    char markup[1024];
    uint64_t checksum;
    int dirty;
    int32_t order;
    uint64_t revision;
} UmiFrontendRenderNodeSnapshot;

/**
 * Represent the frontend render node registry data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendRenderNodeRegistry UmiFrontendRenderNodeRegistry;

/**
 * Initialise frontend render tree registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_render_tree_registry_create(UmiFrontendRenderNodeRegistry **out_registry);
/**
 * Release or reset state held by frontend render tree registry so the same storage can be
 * reused safely.
 */
void umi_frontend_render_tree_registry_destroy(UmiFrontendRenderNodeRegistry *registry);
/**
 * Provide the frontend render tree registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_render_tree_registry_upsert(UmiFrontendRenderNodeRegistry *registry, const UmiFrontendRenderNodeSnapshot *item);
/**
 * Remove frontend render tree registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_render_tree_registry_remove(UmiFrontendRenderNodeRegistry *registry, const char *id);
/**
 * Find frontend render tree registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_render_tree_registry_find(const UmiFrontendRenderNodeRegistry *registry, const char *id, UmiFrontendRenderNodeSnapshot *out_item);
/**
 * Find frontend render tree registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_render_tree_registry_at(const UmiFrontendRenderNodeRegistry *registry, size_t index, UmiFrontendRenderNodeSnapshot *out_item);
/**
 * Return the number of records represented by frontend render tree registry without
 * changing their state.
 */
size_t umi_frontend_render_tree_registry_count(const UmiFrontendRenderNodeRegistry *registry);
/**
 * Provide the frontend render tree registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_render_tree_registry_revision(const UmiFrontendRenderNodeRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_frontend_render_tree_snapshot_validate(const UmiFrontendRenderNodeSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_FRONTEND_RENDER_TREE_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_frontend_render_tree_registry_upsert_many(UmiFrontendRenderNodeRegistry *registry,
    const UmiFrontendRenderNodeSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
