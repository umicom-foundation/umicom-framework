/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/workbench_headless.c
 *
 * PURPOSE:
 *   Render workbench identity, active perspective, active document and aggregate
 *   component counts for headless inspection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "headless_internal.h"

UmiStatus umi_headless_render_workbench(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench)
{
    UmiUiWorkbenchSnapshot snapshot;
    UmiStatus status;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = umi_headless_append(adapter,
        "workbench=%s\nperspective=%s\nactive-document=%s\n",
        snapshot.workbench_id, snapshot.active_perspective,
        snapshot.active_document_view);
    if (status == UMI_STATUS_OK) status = umi_headless_render_panes(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_documents(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_status(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_notifications(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_dialogs(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_selection(adapter, workbench);
    return status;
}
