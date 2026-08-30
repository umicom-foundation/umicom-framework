/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_repository.c
 *
 * PURPOSE:
 *   Implement the test vcs repository behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* VCS provider v2 repository dispatch tests. Sammy Hegab, Umicom Foundation. MIT. */
#include <assert.h>
#include "vcs_fixture.h"
int main(void)
{
    UmiTestVcsFixture fixture = {0}; UmiVcsProvider provider = test_vcs_provider(&fixture); UmiVcsRepository *repository = NULL;
    UmiVcsBranchList *branches = NULL; UmiVcsRemoteList *remotes = NULL; UmiVcsTagList *tags = NULL; char diff[2048];
    assert(umi_vcs_repository_create(".", &provider, &repository) == UMI_STATUS_OK);
    assert((umi_vcs_repository_capabilities(repository) & UMI_VCS_CAPABILITY_DIFF) != 0U);
    assert(umi_vcs_branch_list_create(&branches) == UMI_STATUS_OK); assert(umi_vcs_remote_list_create(&remotes) == UMI_STATUS_OK); assert(umi_vcs_tag_list_create(&tags) == UMI_STATUS_OK);
    assert(umi_vcs_repository_branches(repository, branches) == UMI_STATUS_OK); assert(umi_vcs_repository_remotes(repository, remotes) == UMI_STATUS_OK);
    assert(umi_vcs_repository_tags(repository, tags) == UMI_STATUS_OK); assert(umi_vcs_repository_diff(repository, "src/main.c", 0, diff, sizeof(diff)) == UMI_STATUS_OK);
    assert(umi_vcs_repository_stage_all(repository) == UMI_STATUS_OK); assert(umi_vcs_repository_unstage_all(repository) == UMI_STATUS_OK);
    assert(umi_vcs_repository_discard(repository, "src/main.c") == UMI_STATUS_OK); assert(umi_vcs_repository_fetch(repository) == UMI_STATUS_OK);
    assert(umi_vcs_repository_branch_create(repository, "feature/b44", 1) == UMI_STATUS_OK);
    assert(umi_vcs_repository_branch_checkout(repository, "main") == UMI_STATUS_OK); assert(umi_vcs_repository_branch_delete(repository, "feature/b44", 0) == UMI_STATUS_OK);
    assert(fixture.diff_calls == 1U && fixture.stage_all_calls == 1U && fixture.delete_calls == 1U);
    umi_vcs_tag_list_destroy(tags); umi_vcs_remote_list_destroy(remotes); umi_vcs_branch_list_destroy(branches); umi_vcs_repository_destroy(repository); return 0;
}
