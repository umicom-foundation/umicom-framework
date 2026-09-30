/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/list_model.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral observable list model for reusable workbench views.
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
#ifndef UMICOM_UI_LIST_MODEL_H
#define UMICOM_UI_LIST_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_LIST_MODEL_CAPACITY 1024U

/**
 * Represent the ui list model snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiListModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char label[256];
    char description[512];
    char icon_name[128];
    int visible;
    int enabled;
    int checked;
    int32_t order;
    uint64_t revision;
} UmiUiListModelSnapshot;

/**
 * Represent the ui list model registry data shared with callers of this public contract.
 */
typedef struct UmiUiListModelRegistry UmiUiListModelRegistry;

/**
 * Initialise ui list model registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_list_model_registry_create(UmiUiListModelRegistry **out_registry);
/**
 * Release or reset state held by ui list model registry so the same storage can be reused
 * safely.
 */
void umi_ui_list_model_registry_destroy(UmiUiListModelRegistry *registry);
/**
 * Provide the ui list model registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_list_model_registry_upsert(UmiUiListModelRegistry *registry, const UmiUiListModelSnapshot *item);
/**
 * Remove ui list model registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_list_model_registry_remove(UmiUiListModelRegistry *registry, const char *id);
/**
 * Find ui list model registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_list_model_registry_find(const UmiUiListModelRegistry *registry, const char *id, UmiUiListModelSnapshot *out_item);
/**
 * Find ui list model registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_list_model_registry_at(const UmiUiListModelRegistry *registry, size_t index, UmiUiListModelSnapshot *out_item);
/**
 * Return the number of records represented by ui list model registry without changing
 * their state.
 */
size_t umi_ui_list_model_registry_count(const UmiUiListModelRegistry *registry);
/**
 * Provide the ui list model registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_list_model_registry_revision(const UmiUiListModelRegistry *registry);
/**
 * Provide the ui list model registry set checked operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_list_model_registry_set_checked(UmiUiListModelRegistry *registry, const char *id, int checked);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_list_model_snapshot_validate(const UmiUiListModelSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_LIST_MODEL_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_list_model_registry_upsert_many(UmiUiListModelRegistry *registry,
    const UmiUiListModelSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
