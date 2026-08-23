/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_profiles/powershell.c
 *
 * PURPOSE:
 *   Define the reusable PowerShell terminal profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_profiles/powershell.h"

static const char *const ARGUMENTS[] = {
    "-NoLogo"
};

const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_powershell(void)
{
    static const UmiDeveloperTerminalProfile profile = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperTerminalProfile),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .profile_id = "developer.terminal.powershell",
        .title = "PowerShell",
        .program = "powershell.exe",
        .arguments = ARGUMENTS,
        .argument_count = sizeof(ARGUMENTS) / sizeof(ARGUMENTS[0]),
        .icon_id = "terminal.powershell",
        .windows_only = 1,
        .posix_only = 0,
        .login_shell = 0
    };
    return &profile;
}
