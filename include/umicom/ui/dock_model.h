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

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_UI_DOCK_MODEL_H
#define UMICOM_UI_DOCK_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_DOCK_MODEL_CAPACITY 256U

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

typedef struct UmiUiDockRegistry UmiUiDockRegistry;

UmiStatus umi_ui_dock_model_registry_create(UmiUiDockRegistry **out_registry);
void umi_ui_dock_model_registry_destroy(UmiUiDockRegistry *registry);
UmiStatus umi_ui_dock_model_registry_upsert(UmiUiDockRegistry *registry, const UmiUiDockSnapshot *item);
UmiStatus umi_ui_dock_model_registry_remove(UmiUiDockRegistry *registry, const char *id);
UmiStatus umi_ui_dock_model_registry_find(const UmiUiDockRegistry *registry, const char *id, UmiUiDockSnapshot *out_item);
UmiStatus umi_ui_dock_model_registry_at(const UmiUiDockRegistry *registry, size_t index, UmiUiDockSnapshot *out_item);
size_t umi_ui_dock_model_registry_count(const UmiUiDockRegistry *registry);
uint64_t umi_ui_dock_model_registry_revision(const UmiUiDockRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
