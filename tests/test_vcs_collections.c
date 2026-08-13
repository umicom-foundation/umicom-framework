/* VCS branch, remote, tag and conflict tests. Sammy Hegab, Umicom Foundation. MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"
int main(void)
{
    UmiVcsBranchList *branches = NULL; UmiVcsRemoteList *remotes = NULL; UmiVcsTagList *tags = NULL;
    UmiVcsChangeList *changes = NULL; UmiVcsConflictList *conflicts = NULL; UmiVcsChange change = {0};
    assert(umi_vcs_branch_list_create(&branches) == UMI_STATUS_OK);
    assert(umi_vcs_branch_list_parse(branches, "*\tmain\torigin/main\t[ahead 2, behind 1]\n \tfeature/source-control\t\t\n") == UMI_STATUS_OK);
    assert(umi_vcs_branch_list_count(branches) == 2U); assert(umi_vcs_branch_list_current(branches)->ahead == 2);
    assert(umi_vcs_remote_list_create(&remotes) == UMI_STATUS_OK);
    assert(umi_vcs_remote_list_parse(remotes, "origin https://example.com/a.git (fetch)\norigin ssh://example/a.git (push)\n") == UMI_STATUS_OK);
    assert(umi_vcs_remote_list_count(remotes) == 1U); assert(strstr(umi_vcs_remote_list_at(remotes, 0U)->push_url, "ssh") != NULL);
    assert(umi_vcs_tag_list_create(&tags) == UMI_STATUS_OK);
    assert(umi_vcs_tag_list_parse(tags, "v1.0\tabc123\tRelease one\nv1.1\tdef456\tRelease two\n") == UMI_STATUS_OK);
    assert(umi_vcs_tag_list_find(tags, "v1.1") != NULL);
    assert(umi_vcs_change_list_create(&changes) == UMI_STATUS_OK); assert(umi_vcs_conflict_list_create(&conflicts) == UMI_STATUS_OK);
    (void)strcpy(change.path, "conflict.c"); change.index_state = UMI_VCS_CHANGE_ADDED; change.worktree_state = UMI_VCS_CHANGE_ADDED;
    assert(umi_vcs_change_list_add(changes, &change) == UMI_STATUS_OK); assert(umi_vcs_conflict_list_build(conflicts, changes) == UMI_STATUS_OK);
    assert(umi_vcs_conflict_list_at(conflicts, 0U)->kind == UMI_VCS_CONFLICT_BOTH_ADDED);
    umi_vcs_conflict_list_destroy(conflicts); umi_vcs_change_list_destroy(changes); umi_vcs_tag_list_destroy(tags);
    umi_vcs_remote_list_destroy(remotes); umi_vcs_branch_list_destroy(branches); return 0;
}
