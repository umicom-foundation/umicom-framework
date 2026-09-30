/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/selection_model.h
 *
 * PURPOSE:
 *   Define reusable selection, focus and anchor state independent of a GUI toolkit.
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
#ifndef UMICOM_UI_SELECTION_MODEL_H
#define UMICOM_UI_SELECTION_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_SELECTION_MODEL_CAPACITY 2048U

/**
 * Represent the ui selection model snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiSelectionModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    int selected;
    int focused;
    int anchor;
    int32_t order;
    uint64_t revision;
} UmiUiSelectionModelSnapshot;

/**
 * Represent the ui selection model registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiSelectionModelRegistry UmiUiSelectionModelRegistry;

/**
 * Initialise ui selection model registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_selection_model_registry_create(UmiUiSelectionModelRegistry **out_registry);
/**
 * Release or reset state held by ui selection model registry so the same storage can be
 * reused safely.
 */
void umi_ui_selection_model_registry_destroy(UmiUiSelectionModelRegistry *registry);
/**
 * Provide the ui selection model registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_selection_model_registry_upsert(UmiUiSelectionModelRegistry *registry, const UmiUiSelectionModelSnapshot *item);
/**
 * Remove ui selection model registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_selection_model_registry_remove(UmiUiSelectionModelRegistry *registry, const char *id);
/**
 * Find ui selection model registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_selection_model_registry_find(const UmiUiSelectionModelRegistry *registry, const char *id, UmiUiSelectionModelSnapshot *out_item);
/**
 * Find ui selection model registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_selection_model_registry_at(const UmiUiSelectionModelRegistry *registry, size_t index, UmiUiSelectionModelSnapshot *out_item);
/**
 * Return the number of records represented by ui selection model registry without changing
 * their state.
 */
size_t umi_ui_selection_model_registry_count(const UmiUiSelectionModelRegistry *registry);
/**
 * Provide the ui selection model registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_selection_model_registry_revision(const UmiUiSelectionModelRegistry *registry);
/**
 * Release or reset state held by ui selection model registry so the same storage can be
 * reused safely.
 */
UmiStatus umi_ui_selection_model_registry_clear(UmiUiSelectionModelRegistry *registry);
/**
 * Provide the ui selection model registry select only operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_selection_model_registry_select_only(UmiUiSelectionModelRegistry *registry, const char *id);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_selection_model_snapshot_validate(const UmiUiSelectionModelSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_SELECTION_MODEL_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_selection_model_registry_upsert_many(UmiUiSelectionModelRegistry *registry,
    const UmiUiSelectionModelSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
