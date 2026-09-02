/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_profiles/cmd.c
 *
 * PURPOSE:
 *   Define the reusable Command Prompt terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_profiles/cmd.h"

/*
 * Provide the developer terminal profile cmd operation used by this module and its client
 * applications.
 */
const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_cmd(void)
{
    static const UmiDeveloperTerminalProfile profile = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperTerminalProfile),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .profile_id = "developer.terminal.cmd",
        .title = "Command Prompt",
        .program = "cmd.exe",
        .arguments = NULL,
        .argument_count = 0U,
        .icon_id = "terminal.cmd",
        .windows_only = 1,
        .posix_only = 0,
        .login_shell = 0
    };
    return &profile;
}
