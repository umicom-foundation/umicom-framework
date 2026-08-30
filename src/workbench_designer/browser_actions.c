/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/browser_actions.c
 *
 * PURPOSE:
 *   Validate browser actions and expose destructive or confirmation
 *   requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/browser_actions.h"
#include "internal.h"


void umi_workbench_designer_browser_action_init(
    UmiWorkbenchDesignerBrowserActionPlan *plan,
    UmiWorkbenchDesignerBrowserActionKind kind,
    const char *action_id)
{
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->kind = kind;
    plan->permitted = true;
    if (action_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            plan->action_id, sizeof(plan->action_id), action_id);
    }
}

UmiStatus umi_workbench_designer_browser_action_prepare(
    const UmiWorkbenchDesignerBrowser *browser,
    UmiWorkbenchDesignerBrowserActionPlan *plan)
{
    const UmiWorkbenchDesignerBrowserItem *selected;
    if (browser == NULL || plan == NULL || plan->action_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    selected = umi_workbench_designer_browser_selected(browser);
    switch (plan->kind) {
        case UMI_WORKBENCH_DESIGNER_BROWSER_CREATE:
        case UMI_WORKBENCH_DESIGNER_BROWSER_IMPORT:
            break;
        case UMI_WORKBENCH_DESIGNER_BROWSER_OPEN:
        case UMI_WORKBENCH_DESIGNER_BROWSER_CLONE:
        case UMI_WORKBENCH_DESIGNER_BROWSER_RENAME:
        case UMI_WORKBENCH_DESIGNER_BROWSER_DELETE:
        case UMI_WORKBENCH_DESIGNER_BROWSER_EXPORT:
        case UMI_WORKBENCH_DESIGNER_BROWSER_SET_DEFAULT:
        case UMI_WORKBENCH_DESIGNER_BROWSER_PIN:
        case UMI_WORKBENCH_DESIGNER_BROWSER_SHARE:
            if (selected == NULL) return UMI_STATUS_NOT_FOUND;
            (void)umi_workbench_designer_copy_text(
                plan->layout_id, sizeof(plan->layout_id), selected->layout_id);
            plan->expected_revision = selected->revision;
            if (selected->built_in &&
                (plan->kind == UMI_WORKBENCH_DESIGNER_BROWSER_DELETE ||
                 plan->kind == UMI_WORKBENCH_DESIGNER_BROWSER_RENAME)) {
                plan->permitted = false;
                return UMI_STATUS_PERMISSION_DENIED;
            }
            break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    plan->destructive = plan->kind == UMI_WORKBENCH_DESIGNER_BROWSER_DELETE;
    plan->requires_confirmation = plan->destructive ||
        plan->kind == UMI_WORKBENCH_DESIGNER_BROWSER_SHARE;
    return UMI_STATUS_OK;
}
