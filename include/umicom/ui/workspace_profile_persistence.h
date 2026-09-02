/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_profile_persistence.h
 *
 * PURPOSE:
 *   Define a compact, toolkit-neutral codec for user-created workspace
 *   profiles, including resized chrome and dragged tool-window placements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSPACE_PROFILE_PERSISTENCE_H
#define UMICOM_UI_WORKSPACE_PROFILE_PERSISTENCE_H

#include "umicom/ui/workspace_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WORKSPACE_PROFILE_TEXT_CAPACITY 2048U

/**
 * Write ui workspace profile in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ui_workspace_profile_encode(
    const UmiUiWorkspaceProfileSnapshot *profile,
    char *out_text,
    size_t capacity);
/**
 * Read ui workspace profile into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ui_workspace_profile_decode(
    const char *text,
    UmiUiWorkspaceProfileSnapshot *out_profile);

#ifdef __cplusplus
}
#endif

#endif
