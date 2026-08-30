/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/workspace_runtime_policy.h
 *
 * PURPOSE:
 *   Define startup, checkpoint, focus and background behavior for complete
 *   application workspace recipes independently of native window toolkits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_WORKSPACE_RUNTIME_POLICY_H
#define UMICOM_APPLICATION_PRESENTATION_WORKSPACE_RUNTIME_POLICY_H

#include "umicom/base/status.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_PRESENTATION_WORKSPACE_POLICY_API_VERSION 1U

typedef enum UmiApplicationPresentationStartupPolicy {
    UMI_APPLICATION_PRESENTATION_STARTUP_RESTORE = 1,
    UMI_APPLICATION_PRESENTATION_STARTUP_PRIMARY_FIRST = 2,
    UMI_APPLICATION_PRESENTATION_STARTUP_ALL_VISIBLE = 3
} UmiApplicationPresentationStartupPolicy;

typedef enum UmiApplicationPresentationCheckpointPolicy {
    UMI_APPLICATION_PRESENTATION_CHECKPOINT_NONE = 1,
    UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_CHANGE = 2,
    UMI_APPLICATION_PRESENTATION_CHECKPOINT_PERIODIC = 3,
    UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_EXIT = 4
} UmiApplicationPresentationCheckpointPolicy;

typedef enum UmiApplicationPresentationFocusPolicy {
    UMI_APPLICATION_PRESENTATION_FOCUS_PRIMARY = 1,
    UMI_APPLICATION_PRESENTATION_FOCUS_LAST_ACTIVE = 2,
    UMI_APPLICATION_PRESENTATION_FOCUS_FIRST_VISIBLE = 3
} UmiApplicationPresentationFocusPolicy;

typedef enum UmiApplicationPresentationBackgroundPolicy {
    UMI_APPLICATION_PRESENTATION_BACKGROUND_PAUSED = 1,
    UMI_APPLICATION_PRESENTATION_BACKGROUND_REDUCED = 2,
    UMI_APPLICATION_PRESENTATION_BACKGROUND_FULL = 3
} UmiApplicationPresentationBackgroundPolicy;

typedef struct UmiApplicationPresentationWorkspaceRuntimePolicy {
    uint32_t struct_size;
    uint32_t api_version;
    const char *recipe_id;
    UmiApplicationPresentationStartupPolicy startup_policy;
    UmiApplicationPresentationCheckpointPolicy checkpoint_policy;
    uint32_t checkpoint_interval_seconds;
    UmiApplicationPresentationFocusPolicy focus_policy;
    UmiApplicationPresentationBackgroundPolicy background_policy;
    int restore_focus;
    int share_context;
    int allow_background_commands;
} UmiApplicationPresentationWorkspaceRuntimePolicy;

UmiStatus umi_application_presentation_workspace_runtime_policy_validate(
    const UmiApplicationPresentationWorkspaceRuntimePolicy *policy);
const char *umi_application_presentation_startup_policy_text(
    UmiApplicationPresentationStartupPolicy policy);
const char *umi_application_presentation_checkpoint_policy_text(
    UmiApplicationPresentationCheckpointPolicy policy);
const char *umi_application_presentation_focus_policy_text(
    UmiApplicationPresentationFocusPolicy policy);
const char *umi_application_presentation_background_policy_text(
    UmiApplicationPresentationBackgroundPolicy policy);

#ifdef __cplusplus
}
#endif

#endif
