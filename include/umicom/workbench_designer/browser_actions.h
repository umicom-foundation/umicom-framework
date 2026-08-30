/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/browser_actions.h
 *
 * PURPOSE:
 *   Prepare auditable Layout Browser actions before storage, import or export
 *   side effects occur.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_BROWSER_ACTIONS_H
#define UMICOM_WORKBENCH_DESIGNER_BROWSER_ACTIONS_H

#include "umicom/workbench_designer/browser.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerBrowserActionKind {
    UMI_WORKBENCH_DESIGNER_BROWSER_OPEN = 1,
    UMI_WORKBENCH_DESIGNER_BROWSER_CREATE = 2,
    UMI_WORKBENCH_DESIGNER_BROWSER_CLONE = 3,
    UMI_WORKBENCH_DESIGNER_BROWSER_RENAME = 4,
    UMI_WORKBENCH_DESIGNER_BROWSER_DELETE = 5,
    UMI_WORKBENCH_DESIGNER_BROWSER_IMPORT = 6,
    UMI_WORKBENCH_DESIGNER_BROWSER_EXPORT = 7,
    UMI_WORKBENCH_DESIGNER_BROWSER_SET_DEFAULT = 8,
    UMI_WORKBENCH_DESIGNER_BROWSER_PIN = 9,
    UMI_WORKBENCH_DESIGNER_BROWSER_SHARE = 10
} UmiWorkbenchDesignerBrowserActionKind;

typedef struct UmiWorkbenchDesignerBrowserActionPlan {
    UmiWorkbenchDesignerBrowserActionKind kind;
    char action_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char target_layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char path[UMI_WORKBENCH_DESIGNER_PATH_CAPACITY];
    char actor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    uint64_t expected_revision;
    bool requires_confirmation;
    bool destructive;
    bool permitted;
} UmiWorkbenchDesignerBrowserActionPlan;

void umi_workbench_designer_browser_action_init(UmiWorkbenchDesignerBrowserActionPlan *plan, UmiWorkbenchDesignerBrowserActionKind kind, const char *action_id);
UmiStatus umi_workbench_designer_browser_action_prepare(const UmiWorkbenchDesignerBrowser *browser, UmiWorkbenchDesignerBrowserActionPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
