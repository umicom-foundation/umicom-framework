/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profile.c
 *
 * PURPOSE:
 *   Validate and atomically install reusable application-shell profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/profile.h"

#include <string.h>

UmiStatus umi_application_shell_profile_validate(
    const UmiApplicationShellProfileDefinition *profile)
{
    size_t index;
    size_t other;

    if (profile == NULL ||
        profile->structure_size != sizeof(*profile) ||
        profile->api_version != UMI_APPLICATION_SHELL_API_VERSION ||
        profile->profile_id == NULL || profile->profile_id[0] == '\0' ||
        profile->title == NULL || profile->title[0] == '\0' ||
        profile->description == NULL ||
        profile->contributions == NULL ||
        profile->contribution_count == 0U ||
        profile->contribution_count > UMI_APPLICATION_SHELL_MAX_PROFILE_ITEMS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < profile->contribution_count; ++index) {
        if (umi_application_shell_contribution_validate(
                &profile->contributions[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        for (other = index + 1U;
             other < profile->contribution_count;
             ++other) {
            if (strcmp(profile->contributions[index].contribution_id,
                       profile->contributions[other].contribution_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_application_shell_profile_install(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellProfileDefinition *profile)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_application_shell_profile_validate(profile);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < profile->contribution_count; ++index) {
        status = umi_application_shell_registry_upsert(
            registry,
            &profile->contributions[index]);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
