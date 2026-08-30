/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_terminal_profile_powershell.c
 *
 * PURPOSE:
 *   Verify the built-in PowerShell terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/terminal_profiles/powershell.h"

int main(void)
{
    assert(umi_developer_terminal_profile_validate(
        umi_developer_terminal_profile_powershell()) == UMI_STATUS_OK);
    return 0;
}
