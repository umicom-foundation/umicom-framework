/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/workspace_runtime_policy.c
 *
 * PURPOSE:
 *   Validate complete workspace runtime behavior and expose readable policy
 *   names for diagnostics and developer tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/workspace_runtime_policy.h"

#include "umicom/application/component/recipe_catalogue.h"

/* Check that boolean satisfies its contract before another service relies on it. */
static int boolean_valid(int value)
{
    return value == 0 || value == 1;
}

/*
 * Check that application presentation workspace runtime policy satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_application_presentation_workspace_runtime_policy_validate(
    const UmiApplicationPresentationWorkspaceRuntimePolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || policy->struct_size != sizeof(*policy) ||
        policy->api_version != UMI_APPLICATION_PRESENTATION_WORKSPACE_POLICY_API_VERSION ||
        policy->recipe_id == NULL || policy->recipe_id[0] == '\0' ||
        policy->startup_policy < UMI_APPLICATION_PRESENTATION_STARTUP_RESTORE ||
        policy->startup_policy > UMI_APPLICATION_PRESENTATION_STARTUP_ALL_VISIBLE ||
        policy->checkpoint_policy < UMI_APPLICATION_PRESENTATION_CHECKPOINT_NONE ||
        policy->checkpoint_policy > UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_EXIT ||
        policy->focus_policy < UMI_APPLICATION_PRESENTATION_FOCUS_PRIMARY ||
        policy->focus_policy > UMI_APPLICATION_PRESENTATION_FOCUS_FIRST_VISIBLE ||
        policy->background_policy < UMI_APPLICATION_PRESENTATION_BACKGROUND_PAUSED ||
        policy->background_policy > UMI_APPLICATION_PRESENTATION_BACKGROUND_FULL ||
        !boolean_valid(policy->restore_focus) ||
        !boolean_valid(policy->share_context) ||
        !boolean_valid(policy->allow_background_commands)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((policy->checkpoint_policy == UMI_APPLICATION_PRESENTATION_CHECKPOINT_PERIODIC) !=
        (policy->checkpoint_interval_seconds > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_component_recipe_catalogue_find(policy->recipe_id) != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the application presentation startup policy text operation used by this module
 * and its client applications.
 */
const char *umi_application_presentation_startup_policy_text(
    UmiApplicationPresentationStartupPolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_APPLICATION_PRESENTATION_STARTUP_RESTORE: return "restore";
    case UMI_APPLICATION_PRESENTATION_STARTUP_PRIMARY_FIRST: return "primary-first";
    case UMI_APPLICATION_PRESENTATION_STARTUP_ALL_VISIBLE: return "all-visible";
    default: return "unknown";
    }
}

/*
 * Provide the application presentation checkpoint policy text operation used by this
 * module and its client applications.
 */
const char *umi_application_presentation_checkpoint_policy_text(
    UmiApplicationPresentationCheckpointPolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_APPLICATION_PRESENTATION_CHECKPOINT_NONE: return "none";
    case UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_CHANGE: return "on-change";
    case UMI_APPLICATION_PRESENTATION_CHECKPOINT_PERIODIC: return "periodic";
    case UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_EXIT: return "on-exit";
    default: return "unknown";
    }
}

/*
 * Provide the application presentation focus policy text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_focus_policy_text(
    UmiApplicationPresentationFocusPolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_APPLICATION_PRESENTATION_FOCUS_PRIMARY: return "primary";
    case UMI_APPLICATION_PRESENTATION_FOCUS_LAST_ACTIVE: return "last-active";
    case UMI_APPLICATION_PRESENTATION_FOCUS_FIRST_VISIBLE: return "first-visible";
    default: return "unknown";
    }
}

/*
 * Provide the application presentation background policy text operation used by this
 * module and its client applications.
 */
const char *umi_application_presentation_background_policy_text(
    UmiApplicationPresentationBackgroundPolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_APPLICATION_PRESENTATION_BACKGROUND_PAUSED: return "paused";
    case UMI_APPLICATION_PRESENTATION_BACKGROUND_REDUCED: return "reduced";
    case UMI_APPLICATION_PRESENTATION_BACKGROUND_FULL: return "full";
    default: return "unknown";
    }
}
