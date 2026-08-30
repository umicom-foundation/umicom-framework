/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_profiles/bash.c
 *
 * PURPOSE:
 *   Define the reusable Bash terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_profiles/bash.h"

static const char *const ARGUMENTS[] = {
    "--login"
};

const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_bash(void)
{
    static const UmiDeveloperTerminalProfile profile = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperTerminalProfile),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .profile_id = "developer.terminal.bash",
        .title = "Bash",
        .program = "/bin/bash",
        .arguments = ARGUMENTS,
        .argument_count = sizeof(ARGUMENTS) / sizeof(ARGUMENTS[0]),
        .icon_id = "terminal.bash",
        .windows_only = 0,
        .posix_only = 1,
        .login_shell = 1
    };
    return &profile;
}
