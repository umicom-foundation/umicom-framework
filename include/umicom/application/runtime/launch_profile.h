/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/launch_profile.h
 *
 * PURPOSE:
 *   Define validated product launch preferences without moving process launching into thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_LAUNCH_PROFILE_H
#define UMICOM_APPLICATION_RUNTIME_LAUNCH_PROFILE_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationLaunchProfile {
    uint32_t structure_size;
    const char *application_id;
    const char *layout_id;
    unsigned frontend;
    bool safe_mode;
    bool restore_session;
} UmiApplicationLaunchProfile;

UmiStatus umi_application_launch_profile_default(
    const UmiApplicationExperienceDefinition *experience,
    unsigned frontend,
    UmiApplicationLaunchProfile *out_profile);
UmiStatus umi_application_launch_profile_validate(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationLaunchProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
