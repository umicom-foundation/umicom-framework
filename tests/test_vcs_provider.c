/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_provider.c
 *
 * PURPOSE:
 *   Verify Git provider validation and provider-neutral repository creation.
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
#include "umicom/vcs/git_cli.h"
#include "umicom/vcs/provider.h"

int main(void)
{
    UmiVcsProvider provider;
    int is_repository = 0;
    assert(umi_vcs_git_cli_provider(&provider) == UMI_STATUS_OK);
    assert(umi_vcs_provider_validate(&provider) == UMI_STATUS_OK);
    assert(strcmp(provider.provider_id, "umicom.vcs.git-cli") == 0);
    assert(umi_vcs_git_cli_is_repository(NULL, &is_repository) ==
           UMI_STATUS_INVALID_ARGUMENT);
    if (provider.destroy != NULL) { provider.destroy(provider.instance); }
    return 0;
}
