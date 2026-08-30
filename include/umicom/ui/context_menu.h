/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/context_menu.h
 *
 * PURPOSE:
 *   Define context-menu contributions with commands, grouping and context expressions.
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
#ifndef UMICOM_UI_CONTEXT_MENU_H
#define UMICOM_UI_CONTEXT_MENU_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_CONTEXT_MENU_CAPACITY 1024U

typedef struct UmiUiContextMenuItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char menu_id[128];
    char command_id[128];
    char label[256];
    char when_expression[256];
    char group[128];
    int visible;
    int enabled;
    int separator_before;
    int32_t order;
    uint64_t revision;
} UmiUiContextMenuItemSnapshot;

typedef struct UmiUiContextMenuItemRegistry UmiUiContextMenuItemRegistry;

UmiStatus umi_ui_context_menu_registry_create(UmiUiContextMenuItemRegistry **out_registry);
void umi_ui_context_menu_registry_destroy(UmiUiContextMenuItemRegistry *registry);
UmiStatus umi_ui_context_menu_registry_upsert(UmiUiContextMenuItemRegistry *registry, const UmiUiContextMenuItemSnapshot *item);
UmiStatus umi_ui_context_menu_registry_remove(UmiUiContextMenuItemRegistry *registry, const char *id);
UmiStatus umi_ui_context_menu_registry_find(const UmiUiContextMenuItemRegistry *registry, const char *id, UmiUiContextMenuItemSnapshot *out_item);
UmiStatus umi_ui_context_menu_registry_at(const UmiUiContextMenuItemRegistry *registry, size_t index, UmiUiContextMenuItemSnapshot *out_item);
size_t umi_ui_context_menu_registry_count(const UmiUiContextMenuItemRegistry *registry);
uint64_t umi_ui_context_menu_registry_revision(const UmiUiContextMenuItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
