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

/*
 * Provide the headless render workbench operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_workbench(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench)
{
    UmiUiWorkbenchSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_headless_render_panes(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_headless_render_documents(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_headless_render_status(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_headless_render_notifications(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_headless_render_dialogs(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_headless_render_selection(adapter, workbench);
    return status;
}
