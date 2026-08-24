/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/control_types.c
 *
 * PURPOSE:
 *   Define stable bounded types and repository-control state vocabulary.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/control_types.h"

const char *umi_repository_control_state_text(UmiRepositoryControlState state)
{
    switch (state) {
        case UMI_REPOSITORY_CONTROL_IDLE: return "idle";
        case UMI_REPOSITORY_CONTROL_INSPECTING: return "inspecting";
        case UMI_REPOSITORY_CONTROL_PLANNED: return "planned";
        case UMI_REPOSITORY_CONTROL_STAGING: return "staging";
        case UMI_REPOSITORY_CONTROL_VERIFIED: return "verified";
        case UMI_REPOSITORY_CONTROL_FAILED: return "failed";
        default: return "unknown";
    }
}

const char *umi_repository_control_severity_text(
    UmiRepositoryControlSeverity severity)
{
    switch (severity) {
        case UMI_REPOSITORY_CONTROL_INFO: return "info";
        case UMI_REPOSITORY_CONTROL_WARNING: return "warning";
        case UMI_REPOSITORY_CONTROL_ERROR: return "error";
        default: return "unknown";
    }
}
