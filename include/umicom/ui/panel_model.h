/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/panel_model.h
 *
 * PURPOSE:
 *   Define reusable primary, auxiliary and bottom panel metadata for workbench shells.
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
#ifndef UMICOM_UI_PANEL_MODEL_H
#define UMICOM_UI_PANEL_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PANEL_MODEL_CAPACITY 256U

typedef struct UmiUiPanelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char title[256];
    char location[64];
    char view_id[128];
    char icon_name[128];
    int visible;
    int maximised;
    int32_t order;
    int32_t preferred_size;
    uint64_t revision;
} UmiUiPanelSnapshot;

typedef struct UmiUiPanelRegistry UmiUiPanelRegistry;

UmiStatus umi_ui_panel_model_registry_create(UmiUiPanelRegistry **out_registry);
void umi_ui_panel_model_registry_destroy(UmiUiPanelRegistry *registry);
UmiStatus umi_ui_panel_model_registry_upsert(UmiUiPanelRegistry *registry, const UmiUiPanelSnapshot *item);
UmiStatus umi_ui_panel_model_registry_remove(UmiUiPanelRegistry *registry, const char *id);
UmiStatus umi_ui_panel_model_registry_find(const UmiUiPanelRegistry *registry, const char *id, UmiUiPanelSnapshot *out_item);
UmiStatus umi_ui_panel_model_registry_at(const UmiUiPanelRegistry *registry, size_t index, UmiUiPanelSnapshot *out_item);
UmiStatus umi_ui_panel_model_registry_set_visible(UmiUiPanelRegistry *registry,
                                                    const char *id,
                                                    int visible);
UmiStatus umi_ui_panel_model_registry_toggle_visible(UmiUiPanelRegistry *registry,
                                                     const char *id);
size_t umi_ui_panel_model_registry_count(const UmiUiPanelRegistry *registry);
uint64_t umi_ui_panel_model_registry_revision(const UmiUiPanelRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
