/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/toolbar_model.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral professional toolbar items shared by IDE, trading, media and design workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/toolbar_model.h"

/*
 * Initialise ws toolbar model from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_toolbar_model_init(UmiWsToolbarModel *toolbar) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar != NULL) *toolbar = (UmiWsToolbarModel){0};
}

/* Add ws toolbar model only after its inputs and available capacity have been checked. */
UmiStatus umi_ws_toolbar_model_add(UmiWsToolbarModel *toolbar,
                                   const char *item_id,
                                   const char *label,
                                   const char *command_id,
                                   UmiWsToolbarItemKind kind) {
    UmiWsToolbarItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar == NULL || !umi_ws_id_valid(item_id) || label == NULL || kind < UMI_WS_TOOLBAR_BUTTON || kind > UMI_WS_TOOLBAR_SPACER) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (toolbar->count >= UMI_WS_MAX_TOOLBAR_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &toolbar->items[toolbar->count++];
    *item = (UmiWsToolbarItem){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(item->item_id, sizeof(item->item_id), item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(item->label, sizeof(item->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id != NULL && command_id[0] != '\0') {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_ws_copy_text(item->command_id, sizeof(item->command_id), command_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item->kind = kind;
    item->enabled = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws toolbar model set active operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_toolbar_model_set_active(UmiWsToolbarModel *toolbar, const char *item_id, bool active) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolbar == NULL || !umi_ws_id_valid(item_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < toolbar->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(toolbar->items[index].item_id, item_id) == 0) {
            toolbar->items[index].active = active;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
