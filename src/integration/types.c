/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/types.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/types.h"

/*
 * Provide the integration application state text operation used by this module and its
 * client applications.
 */
const char *umi_integration_application_state_text(UmiIntegrationApplicationState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_INTEGRATION_APP_UNKNOWN: return "unknown";
        case UMI_INTEGRATION_APP_DISCOVERED: return "discovered";
        case UMI_INTEGRATION_APP_AVAILABLE: return "available";
        case UMI_INTEGRATION_APP_STARTING: return "starting";
        case UMI_INTEGRATION_APP_RUNNING: return "running";
        case UMI_INTEGRATION_APP_STOPPING: return "stopping";
        case UMI_INTEGRATION_APP_STOPPED: return "stopped";
        case UMI_INTEGRATION_APP_FAILED: return "failed";
        default: return "invalid";
    }
}

/*
 * Provide the integration message kind text operation used by this module and its client
 * applications.
 */
const char *umi_integration_message_kind_text(UmiIntegrationMessageKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_INTEGRATION_MESSAGE_COMMAND: return "command";
        case UMI_INTEGRATION_MESSAGE_EVENT: return "event";
        case UMI_INTEGRATION_MESSAGE_QUERY: return "query";
        case UMI_INTEGRATION_MESSAGE_RESPONSE: return "response";
        default: return "invalid";
    }
}
