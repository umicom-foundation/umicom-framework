/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/text.c
 *
 * PURPOSE:
 *   Provide shared plain-language explanations for reviews and activity.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/federation/types.h"
const char *UmiDeskFederationReasonText(UmiDeskFederationReason reason)
{
    switch (reason) {
    case UMI_DESK_FEDERATION_READY: return "Ready for a request; application readiness is checked separately";
    case UMI_DESK_FEDERATION_NOT_REGISTERED: return "Application is not registered in this installation";
    case UMI_DESK_FEDERATION_NOT_INSTALLED: return "Application executable is not installed";
    case UMI_DESK_FEDERATION_INCOMPATIBLE: return "Application is not compatible";
    case UMI_DESK_FEDERATION_DISABLED: return "Application is disabled";
    case UMI_DESK_FEDERATION_TRANSITIONING: return "Application is starting or stopping; wait and preview again";
    case UMI_DESK_FEDERATION_SELF: return "Desk is already the host; it is not relaunched";
    case UMI_DESK_FEDERATION_UNKNOWN_STATE: return "Application state is not known";
    default: return "Invalid review reason";
    }
}
const char *UmiDeskFederationStateText(UmiDeskFederationAppState state)
{
    switch (state) {
    case UMI_DESK_FEDERATION_UNKNOWN: return "Unknown";
    case UMI_DESK_FEDERATION_STOPPED: return "Stopped";
    case UMI_DESK_FEDERATION_STARTING: return "Starting";
    case UMI_DESK_FEDERATION_RUNNING: return "Running";
    case UMI_DESK_FEDERATION_ATTENTION: return "Needs attention";
    case UMI_DESK_FEDERATION_FAILED: return "Failed";
    case UMI_DESK_FEDERATION_STOPPING: return "Stopping";
    default: return "Invalid state";
    }
}
const char *UmiDeskFederationActionText(UmiDeskFederationAction action)
{
    switch (action) {
    case UMI_DESK_FEDERATION_START: return "Request start";
    case UMI_DESK_FEDERATION_ACTIVATE: return "Request switch to running application";
    case UMI_DESK_FEDERATION_SKIP: return "Skip";
    default: return "Invalid action";
    }
}
