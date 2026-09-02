/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_profile.c
 *
 * PURPOSE:
 *   Validate reusable terminal profile metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_profile.h"

/*
 * Check that developer terminal profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_developer_terminal_profile_validate(
    const UmiDeveloperTerminalProfile *profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL ||
        profile->structure_size != sizeof(*profile) ||
        profile->api_version != UMI_DEVELOPER_PRODUCTIVITY_API_VERSION ||
        profile->profile_id == NULL || profile->profile_id[0] == '\0' ||
        profile->title == NULL || profile->title[0] == '\0' ||
        profile->program == NULL || profile->program[0] == '\0' ||
        (profile->argument_count > 0U && profile->arguments == NULL) ||
        (profile->windows_only && profile->posix_only)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
