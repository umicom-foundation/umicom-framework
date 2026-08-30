/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/snapshot.c
 *
 * PURPOSE:
 *   Capture designer state through public accessors rather than exposing service
 *   internals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/snapshot.h"
#include "internal.h"


UmiStatus umi_workbench_designer_snapshot_capture(
    const UmiWorkbenchDesignerController *controller,
    UmiWorkbenchDesignerSnapshot *out_snapshot)
{
    UmiWorkbenchDesignerSession *active;
    const UmiWorkbenchLayoutDocument *document;
    const UmiWorkbenchDesignerSelection *selection;
    const UmiWorkbenchDesignerDiagnostics *diagnostics;
    const UmiWorkbenchDesignerStatusModel *status;
    if (controller == NULL || out_snapshot == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->controller_state = controller->state;
    out_snapshot->session_count =
        umi_workbench_designer_service_session_count(controller->service);
    out_snapshot->dirty_session_count =
        umi_workbench_designer_service_dirty_count(controller->service);
    out_snapshot->service_revision =
        umi_workbench_designer_service_revision(controller->service);
    out_snapshot->controller_revision = controller->revision;
    active = umi_workbench_designer_service_active(controller->service);
    if (active == NULL) return UMI_STATUS_OK;
    (void)umi_workbench_designer_copy_text(
        out_snapshot->active_session_id,
        sizeof(out_snapshot->active_session_id),
        umi_workbench_designer_session_id(active));
    document = umi_workbench_designer_session_document_const(active);
    (void)umi_workbench_designer_copy_text(
        out_snapshot->active_layout_id,
        sizeof(out_snapshot->active_layout_id),
        document->identity.layout_id);
    (void)umi_workbench_designer_copy_text(
        out_snapshot->active_layout_name,
        sizeof(out_snapshot->active_layout_name),
        document->name);
    out_snapshot->mode = umi_workbench_designer_session_mode(active);
    out_snapshot->tool = umi_workbench_designer_session_tool(active);
    selection = umi_workbench_designer_session_selection(active);
    diagnostics = umi_workbench_designer_session_diagnostics(active);
    status = umi_workbench_designer_session_status(active);
    out_snapshot->selection_count = selection != NULL ? selection->count : 0U;
    out_snapshot->issue_count = diagnostics != NULL ? diagnostics->count : 0U;
    out_snapshot->collaborator_count = status != NULL
        ? status->collaborator_count : 0U;
    out_snapshot->save_state = status != NULL
        ? status->save_state : UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
    out_snapshot->document_revision = document->version.revision;
    return UMI_STATUS_OK;
}
