/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/import_export.c
 *
 * PURPOSE:
 *   Validate portable layout transfer intent and canonical file extension.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/import_export.h"
#include "internal.h"


/*
 * Initialise workbench designer transfer plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_transfer_plan_init(
    UmiWorkbenchDesignerTransferPlan *plan,
    UmiWorkbenchDesignerTransferKind kind,
    const char *operation_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->kind = kind;
    plan->validate_schema = true;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            plan->operation_id, sizeof(plan->operation_id), operation_id);
    }
}

/*
 * Provide the workbench designer path is layout operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_path_is_layout(const char *path)
{
    const char *extension;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return false;
    extension = strrchr(path, '.');
    return extension != NULL && strcmp(extension, ".umilayout") == 0;
}

/*
 * Check that workbench designer transfer plan satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_designer_transfer_plan_validate(
    const UmiWorkbenchDesignerTransferPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || plan->operation_id[0] == '\0' ||
        plan->path[0] == '\0' || !umi_workbench_designer_path_is_layout(plan->path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->kind == UMI_WORKBENCH_DESIGNER_TRANSFER_EXPORT &&
        plan->layout_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->kind != UMI_WORKBENCH_DESIGNER_TRANSFER_IMPORT &&
        plan->kind != UMI_WORKBENCH_DESIGNER_TRANSFER_EXPORT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
