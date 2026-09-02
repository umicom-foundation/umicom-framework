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

/*
 * Check that application shell profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_shell_profile_validate(
    const UmiApplicationShellProfileDefinition *profile)
{
    size_t index;
    size_t other;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->contribution_count; ++index) {
        /* Apply this operation only while the related capability or state is available. */
        if (umi_application_shell_contribution_validate(
                &profile->contributions[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U;
             other < profile->contribution_count;
             ++other) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(profile->contributions[index].contribution_id,
                       profile->contributions[other].contribution_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the application shell profile install operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_profile_install(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellProfileDefinition *profile)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_application_shell_profile_validate(profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->contribution_count; ++index) {
        status = umi_application_shell_registry_upsert(
            registry,
            &profile->contributions[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
