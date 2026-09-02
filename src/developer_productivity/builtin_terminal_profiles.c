/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/builtin_terminal_profiles.c
 *
 * PURPOSE:
 *   Enumerate and find familiar terminal profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Return the number of records represented by developer builtin terminal profile without
 * changing their state.
 */
size_t umi_developer_builtin_terminal_profile_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find developer builtin terminal profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_at(size_t index)
{
    return index < umi_developer_builtin_terminal_profile_count()
        ? FACTORIES[index]()
        : NULL;
}

/*
 * Find developer builtin terminal profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_find(const char *profile_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_developer_builtin_terminal_profile_count();
         ++index) {
        const UmiDeveloperTerminalProfile *profile = FACTORIES[index]();

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(profile->profile_id, profile_id) == 0) {
            return profile;
        }
    }

    return NULL;
}
