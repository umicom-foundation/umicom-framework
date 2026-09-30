/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/dock_model.h
 *
 * PURPOSE:
 *   Define persistent dock areas and dock groups inspired by mature multi-pane workbenches.
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
#ifndef UMICOM_UI_DOCK_MODEL_H
#define UMICOM_UI_DOCK_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_DOCK_MODEL_CAPACITY 256U

/**
 * Represent the ui dock snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiDockSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char title[256];
    char area[64];
    char group_id[128];
    char active_item_id[128];
    int visible;
    int locked;
    int floating;
    int32_t order;
    int32_t size;
    uint64_t revision;
} UmiUiDockSnapshot;

/**
 * Represent the ui dock registry data shared with callers of this public contract.
 */
typedef struct UmiUiDockRegistry UmiUiDockRegistry;

/**
 * Initialise ui dock model registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_dock_model_registry_create(UmiUiDockRegistry **out_registry);
/**
 * Release or reset state held by ui dock model registry so the same storage can be reused
 * safely.
 */
void umi_ui_dock_model_registry_destroy(UmiUiDockRegistry *registry);
/**
 * Provide the ui dock model registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_dock_model_registry_upsert(UmiUiDockRegistry *registry, const UmiUiDockSnapshot *item);
/**
 * Remove ui dock model registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_dock_model_registry_remove(UmiUiDockRegistry *registry, const char *id);
/**
 * Find ui dock model registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_dock_model_registry_find(const UmiUiDockRegistry *registry, const char *id, UmiUiDockSnapshot *out_item);
/**
 * Find ui dock model registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_dock_model_registry_at(const UmiUiDockRegistry *registry, size_t index, UmiUiDockSnapshot *out_item);
/**
 * Return the number of records represented by ui dock model registry without changing
 * their state.
 */
size_t umi_ui_dock_model_registry_count(const UmiUiDockRegistry *registry);
/**
 * Provide the ui dock model registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_dock_model_registry_revision(const UmiUiDockRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_dock_model_snapshot_validate(const UmiUiDockSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_DOCK_MODEL_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_dock_model_registry_upsert_many(UmiUiDockRegistry *registry,
    const UmiUiDockSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
