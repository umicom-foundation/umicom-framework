/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_behavior.c
 *
 * PURPOSE:
 *   Validate live panel behavior and provide readable policy names for tools,
 *   diagnostics and junior-friendly documentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_behavior.h"

#include "umicom/application/presentation/panel_catalogue.h"

/* Check that boolean satisfies its contract before another service relies on it. */
static int boolean_valid(int value)
{
    return value == 0 || value == 1;
}

/*
 * Check that application presentation surface behavior satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_application_presentation_surface_behavior_validate(
    const UmiApplicationPresentationSurfaceBehavior *behavior)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (behavior == NULL || behavior->struct_size != sizeof(*behavior) ||
        behavior->api_version != UMI_APPLICATION_PRESENTATION_BEHAVIOR_API_VERSION ||
        behavior->component_id == NULL || behavior->component_id[0] == '\0' ||
        behavior->initial_state < UMI_APPLICATION_PRESENTATION_STATE_LOADING ||
        behavior->initial_state > UMI_APPLICATION_PRESENTATION_STATE_PERMISSION_REQUIRED ||
        behavior->refresh_policy < UMI_APPLICATION_PRESENTATION_REFRESH_MANUAL ||
        behavior->refresh_policy > UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING ||
        behavior->selection_policy < UMI_APPLICATION_PRESENTATION_SELECTION_NONE ||
        behavior->selection_policy > UMI_APPLICATION_PRESENTATION_SELECTION_RANGE ||
        behavior->persistence_policy < UMI_APPLICATION_PRESENTATION_PERSISTENCE_TRANSIENT ||
        behavior->persistence_policy > UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE ||
        behavior->command_mode < UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY ||
        behavior->command_mode > UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED ||
        behavior->context_group_id == NULL ||
        !boolean_valid(behavior->lazy_data) ||
        !boolean_valid(behavior->retain_when_hidden) ||
        !boolean_valid(behavior->publish_context) ||
        !boolean_valid(behavior->accept_context)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((behavior->refresh_policy == UMI_APPLICATION_PRESENTATION_REFRESH_INTERVAL ||
         behavior->refresh_policy == UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING) !=
        (behavior->refresh_interval_seconds > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_presentation_panel_catalogue_find(
               behavior->component_id) != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the application presentation refresh policy text operation used by this module
 * and its client applications.
 */
const char *umi_application_presentation_refresh_policy_text(
    UmiApplicationPresentationRefreshPolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_APPLICATION_PRESENTATION_REFRESH_MANUAL: return "manual";
    case UMI_APPLICATION_PRESENTATION_REFRESH_ON_FOCUS: return "on-focus";
    case UMI_APPLICATION_PRESENTATION_REFRESH_INTERVAL: return "interval";
    case UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING: return "streaming";
    default: return "unknown";
    }
}

/*
 * Provide the application presentation selection policy text operation used by this module
 * and its client applications.
 */
const char *umi_application_presentation_selection_policy_text(
    UmiApplicationPresentationSelectionPolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_APPLICATION_PRESENTATION_SELECTION_NONE: return "none";
    case UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE: return "single";
    case UMI_APPLICATION_PRESENTATION_SELECTION_MULTIPLE: return "multiple";
    case UMI_APPLICATION_PRESENTATION_SELECTION_RANGE: return "range";
    default: return "unknown";
    }
}

/*
 * Provide the application presentation persistence policy text operation used by this
 * module and its client applications.
 */
const char *umi_application_presentation_persistence_policy_text(
    UmiApplicationPresentationPersistencePolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_APPLICATION_PRESENTATION_PERSISTENCE_TRANSIENT: return "transient";
    case UMI_APPLICATION_PRESENTATION_PERSISTENCE_SESSION: return "session";
    case UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE: return "workspace";
    default: return "unknown";
    }
}

/*
 * Provide the application presentation command mode text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_command_mode_text(
    UmiApplicationPresentationCommandMode mode)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (mode) {
    case UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY: return "read-only";
    case UMI_APPLICATION_PRESENTATION_COMMAND_EDITABLE: return "editable";
    case UMI_APPLICATION_PRESENTATION_COMMAND_TRANSACTIONAL: return "transactional";
    case UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED: return "guarded";
    default: return "unknown";
    }
}
