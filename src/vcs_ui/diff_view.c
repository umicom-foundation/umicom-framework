/* Framework unified-diff projection. Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs_ui/vcs_ui.h"
#include "view_internal.h"
#include <stdio.h>
UmiStatus umi_vcs_ui_diff_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view)
{
    const UmiVcsDiffDocument *document; UmiVcsDiffDocumentSnapshot snapshot; size_t count, index; UmiStatus status;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_ui_base_view(view_id, "vcs-diff", "Diff Viewer", "Unified working-tree or staged diff with line classifications and hunk metadata.", out_view);
    if (status != UMI_STATUS_OK) return status;
    document = umi_vcs_workspace_diff(workspace); status = umi_vcs_diff_document_snapshot(document, &snapshot);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_string(*out_view, "vcs-diff.path", snapshot.path);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_boolean(*out_view, "vcs-diff.staged", snapshot.staged);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs-diff.additions", (int64_t)snapshot.additions);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs-diff.deletions", (int64_t)snapshot.deletions);
    count = snapshot.lines < UMI_VCS_UI_VISIBLE_ROWS ? snapshot.lines : UMI_VCS_UI_VISIBLE_ROWS;
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_integer(*out_view, "vcs-diff.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiVcsDiffLine *line = umi_vcs_diff_document_line_at(document, index); char key[64], text[UMI_VCS_DIFF_TEXT_CAPACITY + 64U];
        if (line == NULL) continue;
        (void)snprintf(key, sizeof(key), "vcs-diff.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%6zu %6zu  %s", line->old_line, line->new_line, line->text);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 0U, "studio.action.vcs.diff", "Open Diff…", "Load a path diff", 1);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 1U, "studio.action.vcs.stage", "Stage Path…", "Stage a selected path", snapshot.path[0] != '\0');
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(*out_view, 2U, "studio.action.vcs.unstage", "Unstage Path…", "Unstage a selected path", snapshot.path[0] != '\0');
    return status;
}
