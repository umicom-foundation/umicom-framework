/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/toolbar_model.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral professional toolbar items shared by IDE, trading, media and design workstations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/toolbar_model.h"

void umi_ws_toolbar_model_init(UmiWsToolbarModel *toolbar) {
    if (toolbar != NULL) *toolbar = (UmiWsToolbarModel){0};
}

UmiStatus umi_ws_toolbar_model_add(UmiWsToolbarModel *toolbar,
                                   const char *item_id,
                                   const char *label,
                                   const char *command_id,
                                   UmiWsToolbarItemKind kind) {
    UmiWsToolbarItem *item;
    if (toolbar == NULL || !umi_ws_id_valid(item_id) || label == NULL || kind < UMI_WS_TOOLBAR_BUTTON || kind > UMI_WS_TOOLBAR_SPACER) return UMI_STATUS_INVALID_ARGUMENT;
    if (toolbar->count >= UMI_WS_MAX_TOOLBAR_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &toolbar->items[toolbar->count++];
    *item = (UmiWsToolbarItem){0};
    if (umi_ws_copy_text(item->item_id, sizeof(item->item_id), item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(item->label, sizeof(item->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (command_id != NULL && command_id[0] != '\0') {
        if (umi_ws_copy_text(item->command_id, sizeof(item->command_id), command_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item->kind = kind;
    item->enabled = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_toolbar_model_set_active(UmiWsToolbarModel *toolbar, const char *item_id, bool active) {
    size_t index;
    if (toolbar == NULL || !umi_ws_id_valid(item_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < toolbar->count; ++index) {
        if (strcmp(toolbar->items[index].item_id, item_id) == 0) {
            toolbar->items[index].active = active;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
