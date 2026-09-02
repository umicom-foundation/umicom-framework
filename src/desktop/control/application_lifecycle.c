/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/application_lifecycle.c
 * PURPOSE: Implement track requested application lifecycle intent and bounded transition state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/application_lifecycle.h"

#include <string.h>

/*
 * Initialise desktop application lifecycle from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_application_lifecycle_init(UmiDesktopApplicationLifecycleSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_APPLICATION_LIFECYCLE_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop application lifecycle satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_desktop_application_lifecycle_validate(const UmiDesktopApplicationLifecycleSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_APPLICATION_LIFECYCLE_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_lifecycle_set_subject(UmiDesktopApplicationLifecycleSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the desktop application lifecycle set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_lifecycle_set_detail(UmiDesktopApplicationLifecycleSnapshot *value, const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the desktop application lifecycle set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_lifecycle_set_state(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle set priority operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_lifecycle_set_priority(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t priority)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_lifecycle_set_enabled(UmiDesktopApplicationLifecycleSnapshot *value, bool enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle same identity operation used by this module
 * and its client applications.
 */
bool umi_desktop_application_lifecycle_same_identity(const UmiDesktopApplicationLifecycleSnapshot *left, const UmiDesktopApplicationLifecycleSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop application lifecycle transition allowed operation used by this
 * module and its client applications.
 */
bool umi_desktop_application_lifecycle_transition_allowed(UmiDesktopControlLifecycleState from_state, UmiDesktopControlLifecycleState to_state)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (from_state == to_state) return true;
    /* Select the behaviour associated with the requested command or state value. */
    switch (from_state) {
        case UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_RUNNING ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_RUNNING:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPING ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPING:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED;
        default: return false;
    }
}
