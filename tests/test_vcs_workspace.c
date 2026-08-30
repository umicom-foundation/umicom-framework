/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_workspace.c
 *
 * PURPOSE:
 *   Implement the test vcs workspace behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Framework-owned VCS workspace orchestration tests. Sammy Hegab, Umicom Foundation. MIT. */
#include <assert.h>
#include <string.h>
#include "vcs_fixture.h"
int main(void)
{
    UmiTestVcsFixture fixture = {0}; UmiVcsProvider provider = test_vcs_provider(&fixture); UmiVcsWorkspace *workspace = NULL;
    UmiVcsWorkspaceSnapshot snapshot; UmiVcsDiffDocumentSnapshot diff_snapshot; char commit_id[32];
    assert(umi_vcs_workspace_create(".", &provider, 1, &workspace) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_refresh(workspace, 25U) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.branch, "main") == 0 && snapshot.changes == 1U && snapshot.branches == 1U && snapshot.remotes == 1U && snapshot.tags == 1U);
    assert(umi_vcs_workspace_stage(workspace, "src/main.c") == UMI_STATUS_OK); assert(umi_vcs_workspace_unstage(workspace, "src/main.c") == UMI_STATUS_OK);
    assert(umi_vcs_workspace_commit(workspace, "feat: source control", commit_id, sizeof(commit_id)) == UMI_STATUS_OK);
    assert(strcmp(commit_id, "abc1234") == 0); assert(umi_vcs_workspace_fetch(workspace) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_branch_create(workspace, "feature/b44", 1) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_open_diff(workspace, "src/main.c", 0) == UMI_STATUS_OK);
    assert(umi_vcs_diff_document_snapshot(umi_vcs_workspace_diff(workspace), &diff_snapshot) == UMI_STATUS_OK && diff_snapshot.additions == 1U);
    assert(umi_vcs_operation_log_count(umi_vcs_workspace_operations(workspace)) >= 7U);
    umi_vcs_workspace_destroy(workspace); return 0;
}
