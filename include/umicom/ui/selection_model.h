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

typedef struct UmiUiSelectionModelRegistry UmiUiSelectionModelRegistry;

UmiStatus umi_ui_selection_model_registry_create(UmiUiSelectionModelRegistry **out_registry);
void umi_ui_selection_model_registry_destroy(UmiUiSelectionModelRegistry *registry);
UmiStatus umi_ui_selection_model_registry_upsert(UmiUiSelectionModelRegistry *registry, const UmiUiSelectionModelSnapshot *item);
UmiStatus umi_ui_selection_model_registry_remove(UmiUiSelectionModelRegistry *registry, const char *id);
UmiStatus umi_ui_selection_model_registry_find(const UmiUiSelectionModelRegistry *registry, const char *id, UmiUiSelectionModelSnapshot *out_item);
UmiStatus umi_ui_selection_model_registry_at(const UmiUiSelectionModelRegistry *registry, size_t index, UmiUiSelectionModelSnapshot *out_item);
size_t umi_ui_selection_model_registry_count(const UmiUiSelectionModelRegistry *registry);
uint64_t umi_ui_selection_model_registry_revision(const UmiUiSelectionModelRegistry *registry);
UmiStatus umi_ui_selection_model_registry_clear(UmiUiSelectionModelRegistry *registry);
UmiStatus umi_ui_selection_model_registry_select_only(UmiUiSelectionModelRegistry *registry, const char *id);

#ifdef __cplusplus
}
#endif

#endif
