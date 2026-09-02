/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_workspace_coordinator.c
 *
 * PURPOSE:
 *   Verify selection, filtering, commit composition and capability state in
 *   the Framework-owned professional source-control coordinator.
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

#include "vcs_fixture.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestVcsFixture fixture = {0};
    UmiVcsProvider provider = test_vcs_provider(&fixture);
    UmiVcsWorkspace *workspace = NULL;
    UmiVcsWorkspaceCoordinator *coordinator = NULL;
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiVcsChange change;
    UmiVcsCommit commit;
    UmiVcsBranch branch;
    UmiVcsRemote remote;
    UmiVcsDiffLine line;
    UmiVcsOperation operation;
    char commit_id[32];

    assert(umi_vcs_workspace_create(".", &provider, 1, &workspace) ==
           UMI_STATUS_OK);
    assert(umi_vcs_workspace_refresh(workspace, 25U) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_create(workspace, &coordinator) ==
           UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_snapshot(coordinator, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.visible_change_count == 1U);
    assert(snapshot.staged_change_count == 1U);
    assert(strcmp(snapshot.selected_change_path, "src/main.c") == 0);
    assert(strcmp(snapshot.selected_branch_name, "main") == 0);
    assert(strcmp(snapshot.selected_remote_name, "origin") == 0);
    assert(snapshot.can_unstage_selected);
    assert(!snapshot.can_commit);

    assert(umi_vcs_workspace_coordinator_change_at(coordinator, 0U,
                                                    &change) == UMI_STATUS_OK);
    assert(strcmp(change.path, "src/main.c") == 0);
    assert(umi_vcs_workspace_coordinator_commit_at(coordinator, 0U,
                                                    &commit) == UMI_STATUS_OK);
    assert(strcmp(commit.commit_id, "abc123") == 0);
    assert(umi_vcs_workspace_coordinator_branch_at(coordinator, 0U,
                                                    &branch) == UMI_STATUS_OK);
    assert(branch.current);
    assert(umi_vcs_workspace_coordinator_remote_at(coordinator, 0U,
                                                    &remote) == UMI_STATUS_OK);
    assert(strcmp(remote.name, "origin") == 0);

    assert(umi_vcs_workspace_coordinator_set_change_filter(
               coordinator, UMI_VCS_CHANGE_FILTER_STAGED) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_snapshot(coordinator, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.visible_change_count == 1U);
    assert(strcmp(snapshot.change_filter_label, "staged") == 0);
    assert(umi_vcs_workspace_coordinator_set_change_filter(
               coordinator, UMI_VCS_CHANGE_FILTER_UNSTAGED) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_snapshot(coordinator, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.visible_change_count == 0U);

    assert(umi_vcs_workspace_coordinator_set_change_filter(
               coordinator, UMI_VCS_CHANGE_FILTER_ALL) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_select_change(
               coordinator, "src/main.c") == UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_set_commit_message(
               coordinator, "feat: professional source control") ==
           UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_snapshot(coordinator, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.can_commit);
    assert(umi_vcs_workspace_coordinator_open_selected_diff(
               coordinator, 1) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_diff_line_at(coordinator, 0U,
                                                       &line) == UMI_STATUS_OK);
    assert(line.kind == UMI_VCS_DIFF_HEADER);
    assert(umi_vcs_workspace_coordinator_commit(coordinator, commit_id,
                                                 sizeof(commit_id)) ==
           UMI_STATUS_OK);
    assert(strcmp(commit_id, "abc1234") == 0);
    assert(umi_vcs_workspace_coordinator_snapshot(coordinator, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.commit_message[0] == '\0');
    assert(snapshot.operation_count > 0U);
    assert(umi_vcs_workspace_coordinator_operation_at(coordinator, 0U,
                                                       &operation) ==
           UMI_STATUS_OK);

    umi_vcs_workspace_coordinator_destroy(coordinator);
    umi_vcs_workspace_destroy(workspace);
    return 0;
}
