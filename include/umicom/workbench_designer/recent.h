/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/recent.h
 *
 * PURPOSE:
 *   Maintain a bounded most-recently-used layout list for the Layout Browser and
 *   Desk menus.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_RECENT_H
#define UMICOM_WORKBENCH_DESIGNER_RECENT_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerRecentItem {
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    uint64_t opened_at_ms;
    uint64_t revision;
    bool pinned;
} UmiWorkbenchDesignerRecentItem;

typedef struct UmiWorkbenchDesignerRecentList {
    UmiWorkbenchDesignerRecentItem items[32U];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerRecentList;

void umi_workbench_designer_recent_init(UmiWorkbenchDesignerRecentList *list);
UmiStatus umi_workbench_designer_recent_touch(UmiWorkbenchDesignerRecentList *list, const char *layout_id, const char *name, uint64_t opened_at_ms, uint64_t revision);
UmiStatus umi_workbench_designer_recent_pin(UmiWorkbenchDesignerRecentList *list, const char *layout_id, bool pinned);
const UmiWorkbenchDesignerRecentItem *umi_workbench_designer_recent_at(const UmiWorkbenchDesignerRecentList *list, size_t index);

#ifdef __cplusplus
}
#endif

#endif
