/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs_ui/remotes_view.c
 *
 * PURPOSE:
 *   Implement the remotes view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Framework VCS remote and operation projection. Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs_ui/vcs_ui.h"
#include "view_internal.h"
#include <stdio.h>
/*
 * Initialise vcs ui remotes view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_ui_remotes_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view)
{
    const UmiVcsRemoteList *remotes; const UmiVcsOperationLog *operations; size_t count, remote_count, index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_ui_base_view(view_id, "vcs-remotes", "Remotes & Operations", "Remote endpoints and the bounded source-control operation journal.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    remotes = umi_vcs_workspace_remotes(workspace); remote_count = umi_vcs_remote_list_count(remotes);
    count = remote_count > 8U ? 8U : remote_count;
    status = umi_vcs_ui_set_integer(*out_view, "vcs-remotes.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiVcsRemote *remote = umi_vcs_remote_list_at(remotes, index); char key[64], text[UMI_VCS_URL_CAPACITY + UMI_VCS_NAME_CAPACITY + 8U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (remote == NULL) continue;
        (void)snprintf(key, sizeof(key), "vcs-remotes.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s  %s", remote->name, remote->fetch_url);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    operations = umi_vcs_workspace_operations(workspace); count = umi_vcs_operation_log_count(operations);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > 8U) count = 8U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs-operations.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiVcsOperation *operation = umi_vcs_operation_log_at(operations, umi_vcs_operation_log_count(operations) - count + index);
        char key[64], text[UMI_VCS_PATH_CAPACITY + UMI_VCS_MESSAGE_CAPACITY + 96U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (operation == NULL) continue;
        (void)snprintf(key, sizeof(key), "vcs-operations.row.%zu", index);
        (void)snprintf(text, sizeof(text), "#%llu %s [%s] %s — %s",
            (unsigned long long)operation->operation_id, umi_vcs_operation_kind_text(operation->kind),
            umi_vcs_operation_state_text(operation->state), operation->subject, operation->summary);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 0U, "studio.action.vcs.fetch", "Fetch", "Fetch all remotes and prune stale refs", remote_count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 1U, "studio.action.vcs.pull", "Pull", "Fast-forward current branch", remote_count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 2U, "studio.action.vcs.push", "Push", "Push current branch", remote_count > 0U);
    return status;
}
