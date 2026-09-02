/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/runtime_types.c
 *
 * PURPOSE:
 *   Provide readable names for live presentation states and events used in
 *   diagnostics, tools and product-facing status reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/runtime_types.h"

/*
 * Provide the application presentation surface state text operation used by this module
 * and its client applications.
 */
const char *umi_application_presentation_surface_state_text(
    UmiApplicationPresentationSurfaceState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_APPLICATION_PRESENTATION_STATE_DORMANT: return "dormant";
    case UMI_APPLICATION_PRESENTATION_STATE_LOADING: return "loading";
    case UMI_APPLICATION_PRESENTATION_STATE_READY: return "ready";
    case UMI_APPLICATION_PRESENTATION_STATE_EMPTY: return "empty";
    case UMI_APPLICATION_PRESENTATION_STATE_BUSY: return "busy";
    case UMI_APPLICATION_PRESENTATION_STATE_WARNING: return "warning";
    case UMI_APPLICATION_PRESENTATION_STATE_ERROR: return "error";
    case UMI_APPLICATION_PRESENTATION_STATE_OFFLINE: return "offline";
    case UMI_APPLICATION_PRESENTATION_STATE_PERMISSION_REQUIRED:
        return "permission-required";
    default: return "unknown";
    }
}

/*
 * Provide the application presentation surface event text operation used by this module
 * and its client applications.
 */
const char *umi_application_presentation_surface_event_text(
    UmiApplicationPresentationSurfaceEvent event)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (event) {
    case UMI_APPLICATION_PRESENTATION_EVENT_MOUNT: return "mount";
    case UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT: return "unmount";
    case UMI_APPLICATION_PRESENTATION_EVENT_ACTIVATE: return "activate";
    case UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE: return "deactivate";
    case UMI_APPLICATION_PRESENTATION_EVENT_FOCUS: return "focus";
    case UMI_APPLICATION_PRESENTATION_EVENT_REFRESH: return "refresh";
    case UMI_APPLICATION_PRESENTATION_EVENT_COMMAND: return "command";
    case UMI_APPLICATION_PRESENTATION_EVENT_CONTEXT_CHANGED:
        return "context-changed";
    default: return "unknown";
    }
}
