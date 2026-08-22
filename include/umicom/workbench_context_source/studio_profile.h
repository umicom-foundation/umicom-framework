/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/studio_profile.h
 *
 * PURPOSE:
 *   Register the standard Studio interaction sources without moving Studio composition policy into GTK callbacks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_STUDIO_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_STUDIO_PROFILE_H
#include "umicom/workbench_context_source/service.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceStudioProfile {
    const char *application_id;
    const char *development_group_id;
    const char *testing_group_id;
    const char *ai_group_id;
    const char *data_group_id;
} UmiWorkbenchContextSourceStudioProfile;

UmiWorkbenchContextSourceStudioProfile
umi_workbench_context_source_studio_profile_default(void);
UmiStatus umi_workbench_context_source_register_studio_profile(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceStudioProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
