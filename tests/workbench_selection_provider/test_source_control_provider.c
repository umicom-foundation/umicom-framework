/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_source_control_provider.c
 *
 * PURPOSE:
 *   Verify Source Control changes/commits/branches become typed selections without display parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection_provider/source_control.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSourceControlChangeSnapshot change = {0};
    UmiSourceControlCommitSnapshot commit = {0};
    UmiSourceControlBranchSnapshot branch = {0};
    UmiWorkbenchSelection selection;

    change.struct_size = (uint32_t)sizeof(change);
    change.api_version = 1U;
    (void)strcpy(change.id, "change-1");
    (void)strcpy(change.repository_id, "repo");
    (void)strcpy(change.uri, "src/main.c");
    (void)strcpy(change.status, "modified");
    change.staged = 1;
    change.revision = 2U;
    assert(umi_workbench_selection_provider_from_source_control_change(
        &change, "studio", "studio.source-control", "workspace",
        "C:/dev/project", "main", 100U, &selection) == UMI_STATUS_OK);
    assert(selection.kind ==
        UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_CHANGE);

    commit.struct_size = (uint32_t)sizeof(commit);
    commit.api_version = 1U;
    (void)strcpy(commit.id, "commit-1");
    (void)strcpy(commit.repository_id, "repo");
    (void)strcpy(commit.hash, "abcdef");
    (void)strcpy(commit.subject, "Change");
    commit.revision = 3U;
    assert(umi_workbench_selection_provider_from_source_control_commit(
        &commit, "studio", "studio.source-control", "workspace",
        110U, &selection) == UMI_STATUS_OK);
    assert(selection.kind ==
        UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_COMMIT);

    branch.struct_size = (uint32_t)sizeof(branch);
    branch.api_version = 1U;
    (void)strcpy(branch.id, "branch-main");
    (void)strcpy(branch.repository_id, "repo");
    (void)strcpy(branch.name, "main");
    branch.current = 1;
    branch.revision = 4U;
    assert(umi_workbench_selection_provider_from_source_control_branch(
        &branch, "studio", "studio.source-control", "workspace",
        120U, &selection) == UMI_STATUS_OK);
    assert(selection.kind ==
        UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_BRANCH);

    return 0;
}
