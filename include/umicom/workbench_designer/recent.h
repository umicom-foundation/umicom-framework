/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/recent.h
 *
 * PURPOSE:
 *   Maintain a bounded most-recently-used layout list for the Layout Browser and
 *   Desk menus.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_RECENT_H
#define UMICOM_WORKBENCH_DESIGNER_RECENT_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer recent item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerRecentItem {
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    uint64_t opened_at_ms;
    uint64_t revision;
    bool pinned;
} UmiWorkbenchDesignerRecentItem;

/**
 * Represent the workbench designer recent list data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerRecentList {
    UmiWorkbenchDesignerRecentItem items[32U];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerRecentList;

/**
 * Initialise workbench designer recent from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_recent_init(UmiWorkbenchDesignerRecentList *list);
/**
 * Provide the workbench designer recent touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_recent_touch(UmiWorkbenchDesignerRecentList *list, const char *layout_id, const char *name, uint64_t opened_at_ms, uint64_t revision);
/**
 * Provide the workbench designer recent pin operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_recent_pin(UmiWorkbenchDesignerRecentList *list, const char *layout_id, bool pinned);
/**
 * Find workbench designer recent while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerRecentItem *umi_workbench_designer_recent_at(const UmiWorkbenchDesignerRecentList *list, size_t index);

#ifdef __cplusplus
}
#endif

#endif
