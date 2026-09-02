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

#ifdef __cplusplus
}
#endif

#endif
