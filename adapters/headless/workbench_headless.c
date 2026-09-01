/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/workbench_headless.c
 *
 * PURPOSE:
 *   Render workbench identity, active navigation and aggregate component counts for headless inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The headless snapshot now exposes Batch 23 activity/view-container state so CI can validate the same navigation model without GTK4.
 */

#include "headless_internal.h"

UmiStatus umi_headless_render_workbench(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench)
{
    UmiUiWorkbenchSnapshot snapshot;
    UmiStatus status;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_headless_append(
        adapter,
        "workbench=%s\n"
        "perspective=%s\n"
        "active-document=%s\n"
        "activity=%s\n"
        "view-container=%s\n"
        "activities=%zu\n"
        "view-containers=%zu\n"
        "keybindings=%zu\n"
        "context-keys=%zu\n"
        "explorer-nodes=%zu\n",
        snapshot.workbench_id,
        snapshot.active_perspective,
        snapshot.active_document_view,
        snapshot.active_activity,
        snapshot.active_view_container,
        snapshot.activity_count,
        snapshot.view_container_count,
        snapshot.keybinding_count,
        snapshot.context_key_count,
        snapshot.explorer_node_count);

    if (status == UMI_STATUS_OK) status = umi_headless_render_panes(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_documents(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_status(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_notifications(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_dialogs(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_headless_render_selection(adapter, workbench);
    return status;
}
