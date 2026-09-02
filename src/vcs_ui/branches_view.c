/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs_ui/branches_view.c
 *
 * PURPOSE:
 *   Implement the branches view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Framework VCS branch projection. Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs_ui/vcs_ui.h"
#include "view_internal.h"
#include <stdio.h>
/*
 * Initialise vcs ui branches view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_vcs_ui_branches_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view)
{
    const UmiVcsBranchList *branches; size_t count, index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_ui_base_view(view_id, "vcs-branches", "Branches", "Create, inspect, switch and safely delete repository branches.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    branches = umi_vcs_workspace_branches(workspace); count = umi_vcs_branch_list_count(branches);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_VCS_UI_VISIBLE_ROWS) count = UMI_VCS_UI_VISIBLE_ROWS;
    status = umi_vcs_ui_set_integer(*out_view, "vcs-branches.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiVcsBranch *branch = umi_vcs_branch_list_at(branches, index); char key[64], text[UMI_VCS_NAME_CAPACITY * 2U + 96U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (branch == NULL) continue;
        (void)snprintf(key, sizeof(key), "vcs-branches.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %s%s%s  ↑%d ↓%d",
            branch->current ? "●" : "○", branch->name, branch->upstream[0] != '\0' ? " → " : "", branch->upstream,
            branch->ahead, branch->behind);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 0U, "studio.action.vcs.branch-create", "New Branch…", "Create and check out a branch", 1);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 1U, "studio.action.vcs.branch-checkout", "Checkout…", "Switch to an existing branch", count > 0U);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 2U, "studio.action.vcs.branch-delete", "Delete…", "Safely delete a merged branch", count > 1U);
    return status;
}
