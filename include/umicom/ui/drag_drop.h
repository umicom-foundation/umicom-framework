/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/drag_drop.h
 *
 * PURPOSE:
 *   Define toolkit-neutral drag-and-drop intent and payload metadata.
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
#ifndef UMICOM_UI_DRAG_DROP_H
#define UMICOM_UI_DRAG_DROP_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_DRAG_DROP_CAPACITY 256U

typedef struct UmiUiDragDropSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char source_id[128];
    char target_id[128];
    char mime_type[128];
    char payload[512];
    int allowed;
    int copy;
    int move;
    int link;
    uint64_t revision;
} UmiUiDragDropSnapshot;

typedef struct UmiUiDragDropRegistry UmiUiDragDropRegistry;

UmiStatus umi_ui_drag_drop_registry_create(UmiUiDragDropRegistry **out_registry);
void umi_ui_drag_drop_registry_destroy(UmiUiDragDropRegistry *registry);
UmiStatus umi_ui_drag_drop_registry_upsert(UmiUiDragDropRegistry *registry, const UmiUiDragDropSnapshot *item);
UmiStatus umi_ui_drag_drop_registry_remove(UmiUiDragDropRegistry *registry, const char *id);
UmiStatus umi_ui_drag_drop_registry_find(const UmiUiDragDropRegistry *registry, const char *id, UmiUiDragDropSnapshot *out_item);
UmiStatus umi_ui_drag_drop_registry_at(const UmiUiDragDropRegistry *registry, size_t index, UmiUiDragDropSnapshot *out_item);
size_t umi_ui_drag_drop_registry_count(const UmiUiDragDropRegistry *registry);
uint64_t umi_ui_drag_drop_registry_revision(const UmiUiDragDropRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
