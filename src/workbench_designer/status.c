/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/status.c
 *
 * PURPOSE:
 *   Build the status projection without reading GTK widgets or global state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/status.h"
#include "internal.h"


/*
 * Initialise workbench designer status from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_status_init(UmiWorkbenchDesignerStatusModel *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == NULL) return;
    (void)memset(status, 0, sizeof(*status));
    status->mode = UMI_WORKBENCH_DESIGNER_MODE_BROWSE;
    status->tool = UMI_WORKBENCH_DESIGNER_TOOL_SELECT;
    status->save_state = UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
    status->zoom = 1.0;
}

/*
 * Provide the workbench designer status update operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_status_update(
    UmiWorkbenchDesignerStatusModel *status,
    const char *message,
    const char *layout_name,
    UmiWorkbenchDesignerMode mode,
    UmiWorkbenchDesignerTool tool,
    UmiWorkbenchDesignerSaveState save_state,
    UmiWorkbenchDesignerPoint pointer_world,
    double zoom,
    const UmiWorkbenchDesignerSelection *selection,
    const UmiWorkbenchDesignerCollaborationModel *collaboration,
    size_t issue_count,
    bool layout_locked,
    uint64_t document_revision)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == NULL || message == NULL || layout_name == NULL || zoom <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_designer_copy_text(status->message, sizeof(status->message), message) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(status->layout_name, sizeof(status->layout_name), layout_name) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status->mode = mode;
    status->tool = tool;
    status->save_state = save_state;
    status->pointer_world = pointer_world;
    status->zoom = zoom;
    status->selection_count = selection != NULL ? selection->count : 0U;
    status->collaborator_count = collaboration != NULL ? collaboration->count : 0U;
    status->editing_count = collaboration != NULL
        ? umi_workbench_designer_collaboration_editing_count(collaboration) : 0U;
    status->issue_count = issue_count;
    status->layout_locked = layout_locked;
    status->document_revision = document_revision;
    status->revision += 1U;
    return UMI_STATUS_OK;
}
