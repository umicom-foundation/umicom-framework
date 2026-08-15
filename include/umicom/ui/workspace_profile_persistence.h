/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_profile_persistence.h
 *
 * PURPOSE:
 *   Define a compact, toolkit-neutral codec for user-created workspace
 *   profiles, including resized chrome and dragged tool-window placements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSPACE_PROFILE_PERSISTENCE_H
#define UMICOM_UI_WORKSPACE_PROFILE_PERSISTENCE_H

#include "umicom/ui/workspace_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WORKSPACE_PROFILE_TEXT_CAPACITY 2048U

UmiStatus umi_ui_workspace_profile_encode(
    const UmiUiWorkspaceProfileSnapshot *profile,
    char *out_text,
    size_t capacity);
UmiStatus umi_ui_workspace_profile_decode(
    const char *text,
    UmiUiWorkspaceProfileSnapshot *out_profile);

#ifdef __cplusplus
}
#endif

#endif
