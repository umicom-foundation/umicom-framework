/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/studio_profile.h
 *
 * PURPOSE:
 *   Register the standard Studio interaction sources without moving Studio composition policy into GTK callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_STUDIO_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_STUDIO_PROFILE_H
#include "umicom/workbench_context_source/service.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source studio profile data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceStudioProfile {
    const char *application_id;
    const char *development_group_id;
    const char *testing_group_id;
    const char *ai_group_id;
    const char *data_group_id;
} UmiWorkbenchContextSourceStudioProfile;

/**
 * Provide the workbench context source studio profile default operation used by this
 * module and its client applications.
 */
UmiWorkbenchContextSourceStudioProfile
umi_workbench_context_source_studio_profile_default(void);
/**
 * Provide the workbench context source register studio profile operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_register_studio_profile(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceStudioProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
