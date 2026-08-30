/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_terminal_profile_git_bash.c
 *
 * PURPOSE:
 *   Verify the built-in Git Bash terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/terminal_profiles/git_bash.h"

int main(void)
{
    assert(umi_developer_terminal_profile_validate(
        umi_developer_terminal_profile_git_bash()) == UMI_STATUS_OK);
    return 0;
}
