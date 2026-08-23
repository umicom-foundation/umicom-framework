/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_profiles/wsl.c
 *
 * PURPOSE:
 *   Define the reusable Windows Subsystem for Linux terminal profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_profiles/wsl.h"

const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_wsl(void)
{
    static const UmiDeveloperTerminalProfile profile = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperTerminalProfile),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .profile_id = "developer.terminal.wsl",
        .title = "Windows Subsystem for Linux",
        .program = "wsl.exe",
        .arguments = NULL,
        .argument_count = 0U,
        .icon_id = "terminal.wsl",
        .windows_only = 1,
        .posix_only = 0,
        .login_shell = 0
    };
    return &profile;
}
