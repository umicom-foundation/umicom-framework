/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/internal/workspace_profile_owner.h
 *
 * PURPOSE:
 * Define the privileged Framework-owner operation used to install or refresh
 * built-in workspace profiles without exposing that authority to applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_INTERNAL_WORKSPACE_PROFILE_OWNER_H
#define UMICOM_UI_INTERNAL_WORKSPACE_PROFILE_OWNER_H

#include "umicom/ui/workspace_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Install or refresh a Framework-owned built-in profile. This explicit path
 * cannot overwrite a user profile and preserves the active-profile state. */
UmiStatus umi_ui_workspace_profile_model_install_built_in(
    UmiUiWorkspaceProfileModel *model,
    const UmiUiWorkspaceProfileSnapshot *profile);

#ifdef __cplusplus
}
#endif

#endif
