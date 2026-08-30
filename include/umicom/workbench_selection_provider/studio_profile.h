/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/studio_profile.h
 *
 * PURPOSE:
 *   Register Framework-owned structured selection providers for Studio project, Problems, Source Control and Test Explorer models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_STUDIO_PROFILE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_STUDIO_PROFILE_H

#include "umicom/workbench_selection_provider/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderStudioProfile {
    const char *application_id;
    const char *project_panel_id;
    const char *problems_panel_id;
    const char *source_control_panel_id;
    const char *test_panel_id;
    const char *project_source_id;
    const char *problems_source_id;
    const char *source_control_source_id;
    const char *test_source_id;
} UmiWorkbenchSelectionProviderStudioProfile;

UmiWorkbenchSelectionProviderStudioProfile
umi_workbench_selection_provider_studio_profile_default(void);

UmiStatus umi_workbench_selection_provider_register_studio_profile(
    UmiWorkbenchSelectionProviderService *service,
    const UmiWorkbenchSelectionProviderStudioProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
