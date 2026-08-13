/* Framework VCS branch projection. Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs_ui/vcs_ui.h"
#include "view_internal.h"
#include <stdio.h>
UmiStatus umi_vcs_ui_branches_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view)
{
    const UmiVcsBranchList *branches; size_t count, index; UmiStatus status;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_ui_base_view(view_id, "vcs-branches", "Branches", "Create, inspect, switch and safely delete repository branches.", out_view);
    if (status != UMI_STATUS_OK) return status;
    branches = umi_vcs_workspace_branches(workspace); count = umi_vcs_branch_list_count(branches);
    if (count > UMI_VCS_UI_VISIBLE_ROWS) count = UMI_VCS_UI_VISIBLE_ROWS;
    status = umi_vcs_ui_set_integer(*out_view, "vcs-branches.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiVcsBranch *branch = umi_vcs_branch_list_at(branches, index); char key[64], text[UMI_VCS_NAME_CAPACITY * 2U + 96U];
        if (branch == NULL) continue;
        (void)snprintf(key, sizeof(key), "vcs-branches.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s %s%s%s  ↑%d ↓%d",
            branch->current ? "●" : "○", branch->name, branch->upstream[0] != '\0' ? " → " : "", branch->upstream,
            branch->ahead, branch->behind);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 0U, "studio.action.vcs.branch-create", "New Branch…", "Create and check out a branch", 1);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 1U, "studio.action.vcs.branch-checkout", "Checkout…", "Switch to an existing branch", count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 2U, "studio.action.vcs.branch-delete", "Delete…", "Safely delete a merged branch", count > 1U);
    return status;
}
