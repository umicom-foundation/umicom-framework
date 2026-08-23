/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/builtin_terminal_profiles.c
 *
 * PURPOSE:
 *   Enumerate and find familiar terminal profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/builtin_terminal_profiles.h"

#include <string.h>

#include "umicom/developer_productivity/terminal_profiles/profiles.h"

typedef const UmiDeveloperTerminalProfile *(*TerminalProfileFactory)(void);

static const TerminalProfileFactory FACTORIES[] = {
    umi_developer_terminal_profile_powershell,
    umi_developer_terminal_profile_cmd,
    umi_developer_terminal_profile_ucrt64_bash,
    umi_developer_terminal_profile_git_bash,
    umi_developer_terminal_profile_wsl,
    umi_developer_terminal_profile_bash
};

size_t umi_developer_builtin_terminal_profile_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_at(size_t index)
{
    return index < umi_developer_builtin_terminal_profile_count()
        ? FACTORIES[index]()
        : NULL;
}

const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_find(const char *profile_id)
{
    size_t index;

    if (profile_id == NULL) return NULL;

    for (index = 0U;
         index < umi_developer_builtin_terminal_profile_count();
         ++index) {
        const UmiDeveloperTerminalProfile *profile = FACTORIES[index]();

        if (strcmp(profile->profile_id, profile_id) == 0) {
            return profile;
        }
    }

    return NULL;
}
