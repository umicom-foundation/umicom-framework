/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs_ui/source_control_view.c
 *
 * PURPOSE:
 *   Implement the source control view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Framework Source Control Centre working-tree projection. Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs_ui/vcs_ui.h"
#include "view_internal.h"
#include <stdio.h>
/*
 * Initialise vcs ui source control view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_vcs_ui_source_control_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceSnapshot snapshot; const UmiVcsChangeList *changes; size_t count, index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_ui_base_view(view_id, "source-control", "Source Control",
        "Framework-owned working tree, index, conflicts and repository synchronization.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_workspace_snapshot(workspace, &snapshot); changes = umi_vcs_workspace_changes(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_boolean(*out_view, "vcs.available", snapshot.available);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_string(*out_view, "vcs.branch", snapshot.branch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_string(*out_view, "vcs.upstream", snapshot.upstream);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs.ahead", snapshot.ahead);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs.behind", snapshot.behind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs.staged", (int64_t)snapshot.staged);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs.conflicts", (int64_t)snapshot.conflicts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs.capabilities", (int64_t)snapshot.capabilities);
    count = snapshot.changes < UMI_VCS_UI_VISIBLE_ROWS ? snapshot.changes : UMI_VCS_UI_VISIBLE_ROWS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiVcsChange *change = umi_vcs_change_list_at(changes, index); char key[64], text[UMI_VCS_PATH_CAPACITY + 96U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (change == NULL) continue;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %s | index: %s | worktree: %s",
            change->staged ? "[staged]" : "[change]", change->path,
            umi_vcs_change_state_text(change->index_state), umi_vcs_change_state_text(change->worktree_state));
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 0U, "studio.action.vcs.refresh", "Refresh", "Refresh all repository models", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 1U, "studio.action.vcs.stage-all", "Stage All", "Stage every working-tree change", snapshot.changes > snapshot.staged);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 2U, "studio.action.vcs.unstage-all", "Unstage All", "Remove every indexed change", snapshot.staged > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 3U, "studio.action.vcs.commit", "Commit…", "Create a commit from staged changes", snapshot.staged > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 4U, "studio.action.vcs.fetch", "Fetch", "Fetch and prune remote references", snapshot.remotes > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 5U, "studio.action.vcs.pull", "Pull", "Fast-forward the current branch", snapshot.upstream[0] != '\0');
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 6U, "studio.action.vcs.push", "Push", "Push the current branch", snapshot.remotes > 0U);
    return status;
}
