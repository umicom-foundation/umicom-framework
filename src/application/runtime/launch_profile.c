/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/launch_profile.c
 *
 * PURPOSE:
 *   Create and validate launch profiles against the canonical application experience.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/launch_profile.h"

#include <string.h>

UmiStatus umi_application_launch_profile_default(
    const UmiApplicationExperienceDefinition *experience,
    unsigned frontend,
    UmiApplicationLaunchProfile *out_profile)
{
    if (experience == NULL || out_profile == NULL || frontend == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_profile, 0, sizeof(*out_profile));
    out_profile->structure_size = sizeof(*out_profile);
    out_profile->application_id = experience->application_id;
    out_profile->layout_id = experience->default_layout_id;
    out_profile->frontend = frontend;
    out_profile->restore_session = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_launch_profile_validate(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationLaunchProfile *profile)
{
    if (experience == NULL || profile == NULL ||
        profile->structure_size != sizeof(*profile) ||
        profile->application_id == NULL || profile->layout_id == NULL ||
        profile->frontend == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(profile->application_id, experience->application_id) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_experience_layout_find(experience, profile->layout_id) != NULL
        ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
