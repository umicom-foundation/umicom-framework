/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/command_catalogue.c
 *
 * PURPOSE:
 *   Define stable workbench commands without application-specific handlers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/command_catalogue.h"

#include <string.h>

#define COMMAND(identifier, title, group, detail, mutates, dangerous, compare, merge, selection) \
    { (uint32_t)sizeof(UmiVcsWorkbenchCommand), UMI_VCS_WORKBENCH_API_VERSION, \
      identifier, title, group, detail, mutates, dangerous, compare, merge, selection }

static const UmiVcsWorkbenchCommand COMMANDS[] = {
    COMMAND("vcs.workbench.compare.files", "Compare Files...", "Compare", "Open a two-way file comparison.", 0, 0, 0, 0, 0),
    COMMAND("vcs.workbench.compare.folders", "Compare Folders...", "Compare", "Open a directory comparison.", 0, 0, 0, 0, 0),
    COMMAND("vcs.workbench.compare.revisions", "Compare Revisions...", "Compare", "Compare two repository revisions.", 0, 0, 0, 0, 0),
    COMMAND("vcs.workbench.view.side-by-side", "Side by Side", "Compare", "Use synchronized side-by-side presentation.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.view.inline", "Inline", "Compare", "Use unified inline presentation.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.view.linked-scroll", "Linked Scrolling", "Compare", "Toggle linked scrolling between comparison sides.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.change.next", "Next Difference", "Compare", "Navigate to the next changed hunk.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.change.previous", "Previous Difference", "Compare", "Navigate to the previous changed hunk.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.whitespace.exact", "Compare Whitespace", "Compare", "Treat whitespace as significant.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.whitespace.ignore", "Ignore Whitespace", "Compare", "Ignore whitespace-only changes.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.merge.use-base", "Use Base", "Merge", "Resolve the selected block with base content.", 1, 0, 0, 1, 1),
    COMMAND("vcs.workbench.merge.use-ours", "Use Ours", "Merge", "Resolve the selected block with ours.", 1, 0, 0, 1, 1),
    COMMAND("vcs.workbench.merge.use-theirs", "Use Theirs", "Merge", "Resolve the selected block with theirs.", 1, 0, 0, 1, 1),
    COMMAND("vcs.workbench.merge.use-both", "Use Both", "Merge", "Resolve the selected block with both sides.", 1, 0, 0, 1, 1),
    COMMAND("vcs.workbench.merge.finish", "Finish Merge", "Merge", "Finish a fully resolved merge.", 1, 0, 0, 1, 0),
    COMMAND("vcs.workbench.merge.abort", "Abort Merge...", "Merge", "Abort and restore the merge checkpoint.", 1, 1, 0, 1, 0),
    COMMAND("vcs.workbench.stage.hunk", "Stage Hunk", "Source Control", "Select the current hunk for staging.", 1, 0, 1, 0, 1),
    COMMAND("vcs.workbench.unstage.hunk", "Unstage Hunk", "Source Control", "Select the current hunk for unstaging.", 1, 0, 1, 0, 1),
    COMMAND("vcs.workbench.stage.selected", "Apply Selected Hunks", "Source Control", "Apply the reviewed partial-stage plan.", 1, 0, 1, 0, 1),
    COMMAND("vcs.workbench.review.comment", "Add Review Comment...", "Review", "Add an inline review thread.", 1, 0, 1, 0, 1),
    COMMAND("vcs.workbench.review.resolve", "Resolve Review Thread", "Review", "Resolve the selected review thread.", 1, 0, 1, 0, 1),
    COMMAND("vcs.workbench.review.approve", "Approve", "Review", "Approve when no review threads remain unresolved.", 1, 0, 1, 0, 0),
    COMMAND("vcs.workbench.review.request-changes", "Request Changes", "Review", "Record a changes-requested decision.", 1, 0, 1, 0, 0),
    COMMAND("vcs.workbench.export.patch", "Export Patch...", "Compare", "Export selected differences as a patch.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.export.report", "Export Comparison Report...", "Compare", "Export a comparison report with metadata.", 0, 0, 1, 0, 0),
    COMMAND("vcs.workbench.refresh", "Refresh Comparison", "Compare", "Refresh content through the active provider.", 0, 0, 1, 0, 0)
};

#undef COMMAND

size_t umi_vcs_workbench_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiVcsWorkbenchCommand *umi_vcs_workbench_command_at(size_t index)
{
    return index < umi_vcs_workbench_command_count() ? &COMMANDS[index] : NULL;
}

const UmiVcsWorkbenchCommand *umi_vcs_workbench_command_find(
    const char *command_id)
{
    size_t index;
    if (command_id == NULL) return NULL;
    for (index = 0U; index < umi_vcs_workbench_command_count(); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0) {
            return &COMMANDS[index];
        }
    }
    return NULL;
}
