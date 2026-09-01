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

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_LIST_MODEL_CAPACITY 1024U

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

typedef struct UmiUiListModelRegistry UmiUiListModelRegistry;

UmiStatus umi_ui_list_model_registry_create(UmiUiListModelRegistry **out_registry);
void umi_ui_list_model_registry_destroy(UmiUiListModelRegistry *registry);
UmiStatus umi_ui_list_model_registry_upsert(UmiUiListModelRegistry *registry, const UmiUiListModelSnapshot *item);
UmiStatus umi_ui_list_model_registry_remove(UmiUiListModelRegistry *registry, const char *id);
UmiStatus umi_ui_list_model_registry_find(const UmiUiListModelRegistry *registry, const char *id, UmiUiListModelSnapshot *out_item);
UmiStatus umi_ui_list_model_registry_at(const UmiUiListModelRegistry *registry, size_t index, UmiUiListModelSnapshot *out_item);
size_t umi_ui_list_model_registry_count(const UmiUiListModelRegistry *registry);
uint64_t umi_ui_list_model_registry_revision(const UmiUiListModelRegistry *registry);
UmiStatus umi_ui_list_model_registry_set_checked(UmiUiListModelRegistry *registry, const char *id, int checked);

#ifdef __cplusplus
}
#endif

#endif
