/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/navigation_item.c
 *
 * PURPOSE:
 *   Define renderer-neutral navigation items with command routing and priority metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/navigation_item.h"
#include <string.h>

/* Store a command-routed navigation item without toolkit-specific callbacks. */
UmiStatus umi_adaptive_navigation_item_init(UmiAdaptiveNavigationItem *item,
                                            const char *item_id,
                                            const char *label,
                                            const char *command_id,
                                            uint32_t priority,
                                            int pinned)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || item_id == NULL || label == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(item, 0, sizeof *item);
    status = umi_adaptive_copy_text(item->item_id, sizeof item->item_id, item_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_adaptive_copy_text(item->label, sizeof item->label, label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_adaptive_copy_text(item->command_id, sizeof item->command_id, command_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    item->priority = priority;
    item->pinned = pinned != 0;
    return UMI_STATUS_OK;
}
