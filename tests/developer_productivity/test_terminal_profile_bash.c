/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_terminal_profile_bash.c
 *
 * PURPOSE:
 *   Verify the built-in Bash terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/terminal_profiles/bash.h"

int main(void)
{
    assert(umi_developer_terminal_profile_validate(
        umi_developer_terminal_profile_bash()) == UMI_STATUS_OK);
    return 0;
}
