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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/control_types.h"

/*
 * Provide the repository control state text operation used by this module and its client
 * applications.
 */
const char *umi_repository_control_state_text(UmiRepositoryControlState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the repository control severity text operation used by this module and its
 * client applications.
 */
const char *umi_repository_control_severity_text(
    UmiRepositoryControlSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_REPOSITORY_CONTROL_INFO: return "info";
        case UMI_REPOSITORY_CONTROL_WARNING: return "warning";
        case UMI_REPOSITORY_CONTROL_ERROR: return "error";
        default: return "unknown";
    }
}
