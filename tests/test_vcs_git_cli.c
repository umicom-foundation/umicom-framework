/*-----------------------------------------------------------------------------
 * End-to-end Git CLI provider v2 test against an isolated local repository.
 * No network, shell interpolation or user Git configuration is required.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"
static void run_git(const char *root, const char *const *arguments, size_t count)
{
    UmiProcessRequest request = {0}; UmiProcessResult result;
    request.program = "git"; request.arguments = arguments; request.argument_count = count;
    request.working_directory = root; request.capture_stdout = 1; request.capture_stderr = 1;
    assert(umi_process_execute(&request, &result) == UMI_STATUS_OK); assert(result.exit_code == 0);
}
int main(void)
{
    const char *init[] = {"init", "-b", "main"}; const char *name[] = {"config", "user.name", "Umicom Test"};
    const char *email[] = {"config", "user.email", "test@umicom.local"}; const char *add[] = {"add", "source.c"};
    const char *commit[] = {"commit", "-m", "initial"}; const char *tag[] = {"tag", "v0.1.0"};
    UmiVcsWorkspace *workspace = NULL; UmiVcsWorkspaceSnapshot snapshot; UmiVcsDiffDocumentSnapshot diff;
    char temporary[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], commit_id[UMI_VCS_ID_CAPACITY];
    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    assert(umi_fs_join(root, sizeof(root), temporary, "umicom-vcs-git-cli") == UMI_STATUS_OK);
    (void)umi_fs_remove_tree(root); assert(umi_fs_make_directories(root) == UMI_STATUS_OK); run_git(root, init, 3U);
    run_git(root, name, 3U); run_git(root, email, 3U); assert(umi_fs_join(source, sizeof(source), root, "source.c") == UMI_STATUS_OK);
    assert(umi_fs_write_text(source, "int answer(void) { return 42; }\n") == UMI_STATUS_OK); run_git(root, add, 2U); run_git(root, commit, 3U); run_git(root, tag, 2U);
    assert(umi_vcs_workspace_create_git(root, &workspace) == UMI_STATUS_OK); assert(umi_vcs_workspace_refresh(workspace, 20U) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK); assert(snapshot.available && strcmp(snapshot.branch, "main") == 0);
    assert(snapshot.commits == 1U && snapshot.branches == 1U && snapshot.tags == 1U);
    assert(umi_vcs_workspace_branch_create(workspace, "feature/batch-44", 0) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_branch_delete(workspace, "feature/batch-44", 0) == UMI_STATUS_OK);
    assert(umi_fs_write_text(source, "int answer(void) { return 44; }\n") == UMI_STATUS_OK);
    assert(umi_vcs_workspace_refresh(workspace, 20U) == UMI_STATUS_OK); assert(umi_vcs_workspace_open_diff(workspace, "source.c", 0) == UMI_STATUS_OK);
    assert(umi_vcs_diff_document_snapshot(umi_vcs_workspace_diff(workspace), &diff) == UMI_STATUS_OK);
    assert(diff.additions == 1U && diff.deletions == 1U); assert(umi_vcs_workspace_stage_all(workspace) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_commit(workspace, "feat: verify batch 44", commit_id, sizeof(commit_id)) == UMI_STATUS_OK);
    assert(commit_id[0] != '\0'); assert(umi_vcs_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK && snapshot.changes == 0U);
    umi_vcs_workspace_destroy(workspace); assert(umi_fs_remove_tree(root) == UMI_STATUS_OK); return 0;
}
