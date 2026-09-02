/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs_ui/history_view.c
 *
 * PURPOSE:
 *   Implement the history view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Framework VCS history projection. Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs_ui/vcs_ui.h"
#include "view_internal.h"
#include <stdio.h>
/*
 * Initialise vcs ui history view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_ui_history_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view)
{
    const UmiVcsHistory *history; size_t count, index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_ui_base_view(view_id, "vcs-history", "Repository History", "Framework-owned commit history for the active repository.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    history = umi_vcs_workspace_history(workspace); count = umi_vcs_history_count(history);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_VCS_UI_VISIBLE_ROWS) count = UMI_VCS_UI_VISIBLE_ROWS;
    status = umi_vcs_ui_set_integer(*out_view, "vcs-history.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiVcsCommit *commit = umi_vcs_history_at(history, index); char key[64], text[UMI_VCS_MESSAGE_CAPACITY + (UMI_VCS_NAME_CAPACITY * 2U) + UMI_VCS_ID_CAPACITY + 32U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (commit == NULL) continue;
        (void)snprintf(key, sizeof(key), "vcs-history.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%.12s  %s — %s <%s>", commit->commit_id, commit->subject, commit->author, commit->email);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 0U, "studio.action.vcs.refresh", "Refresh", "Refresh repository history", 1);
    return status;
}
