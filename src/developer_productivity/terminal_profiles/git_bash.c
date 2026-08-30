/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_profiles/git_bash.c
 *
 * PURPOSE:
 *   Define the reusable Git Bash terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_profiles/git_bash.h"

static const char *const ARGUMENTS[] = {
    "--login"
};

const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_git_bash(void)
{
    static const UmiDeveloperTerminalProfile profile = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperTerminalProfile),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .profile_id = "developer.terminal.git-bash",
        .title = "Git Bash",
        .program = "C:/Program Files/Git/bin/bash.exe",
        .arguments = ARGUMENTS,
        .argument_count = sizeof(ARGUMENTS) / sizeof(ARGUMENTS[0]),
        .icon_id = "terminal.git-bash",
        .windows_only = 1,
        .posix_only = 0,
        .login_shell = 1
    };
    return &profile;
}
