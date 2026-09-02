/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/navigation_item.h
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

#ifndef UMICOM_UI_ADAPTIVE_NAVIGATION_ITEM_H
#define UMICOM_UI_ADAPTIVE_NAVIGATION_ITEM_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive navigation item data shared with callers of this public contract.
 */
typedef struct UmiAdaptiveNavigationItem {
    char item_id[UMI_ADAPTIVE_ID_CAPACITY];
    char label[UMI_ADAPTIVE_TEXT_CAPACITY];
    char command_id[UMI_ADAPTIVE_ID_CAPACITY];
    uint32_t priority;
    int pinned;
} UmiAdaptiveNavigationItem;
/* Initialise one navigation item with stable command identity. */
UmiStatus umi_adaptive_navigation_item_init(UmiAdaptiveNavigationItem *item,
                                            const char *item_id,
                                            const char *label,
                                            const char *command_id,
                                            uint32_t priority,
                                            int pinned);

#ifdef __cplusplus
}
#endif
#endif
