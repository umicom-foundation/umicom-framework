/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_terminal_profile_ucrt64_bash.c
 *
 * PURPOSE:
 *   Verify the built-in MSYS2 UCRT64 Bash terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/terminal_profiles/ucrt64_bash.h"

int main(void)
{
    assert(umi_developer_terminal_profile_validate(
        umi_developer_terminal_profile_ucrt64_bash()) == UMI_STATUS_OK);
    return 0;
}
