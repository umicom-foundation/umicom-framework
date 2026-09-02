/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_terminal_profile_cmd.c
 *
 * PURPOSE:
 *   Verify the built-in Command Prompt terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/terminal_profiles/cmd.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(umi_developer_terminal_profile_validate(
        umi_developer_terminal_profile_cmd()) == UMI_STATUS_OK);
    return 0;
}
