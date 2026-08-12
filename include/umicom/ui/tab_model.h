/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/tab_model.h
 *
 * PURPOSE:
 *   Define editor and tool tabs with preview, pinning, dirty-state and grouping semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_UI_TAB_MODEL_H
#define UMICOM_UI_TAB_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_TAB_MODEL_CAPACITY 1024U

typedef struct UmiUiTabSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char group_id[128];
    char title[256];
    char resource[512];
    char icon_name[128];
    int active;
    int pinned;
    int preview;
    int dirty;
    int closable;
    int32_t order;
    uint64_t revision;
} UmiUiTabSnapshot;

typedef struct UmiUiTabRegistry UmiUiTabRegistry;

UmiStatus umi_ui_tab_model_registry_create(UmiUiTabRegistry **out_registry);
void umi_ui_tab_model_registry_destroy(UmiUiTabRegistry *registry);
UmiStatus umi_ui_tab_model_registry_upsert(UmiUiTabRegistry *registry, const UmiUiTabSnapshot *item);
UmiStatus umi_ui_tab_model_registry_remove(UmiUiTabRegistry *registry, const char *id);
UmiStatus umi_ui_tab_model_registry_find(const UmiUiTabRegistry *registry, const char *id, UmiUiTabSnapshot *out_item);
UmiStatus umi_ui_tab_model_registry_at(const UmiUiTabRegistry *registry, size_t index, UmiUiTabSnapshot *out_item);
UmiStatus umi_ui_tab_model_registry_activate(UmiUiTabRegistry *registry,
                                               const char *id);
UmiStatus umi_ui_tab_model_registry_set_dirty(UmiUiTabRegistry *registry,
                                              const char *id,
                                              int dirty);
size_t umi_ui_tab_model_registry_count(const UmiUiTabRegistry *registry);
uint64_t umi_ui_tab_model_registry_revision(const UmiUiTabRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
