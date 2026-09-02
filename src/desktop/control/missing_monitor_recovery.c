/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/missing_monitor_recovery.c
 * PURPOSE: Implement describe fallback behaviour when a saved monitor is no longer present.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/missing_monitor_recovery.h"

#include <string.h>

/*
 * Initialise desktop missing monitor recovery from caller-provided values so later
 * operations receive a known state.
 */
void umi_desktop_missing_monitor_recovery_init(UmiDesktopMissingMonitorRecoverySnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_MISSING_MONITOR_RECOVERY_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop missing monitor recovery satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_desktop_missing_monitor_recovery_validate(const UmiDesktopMissingMonitorRecoverySnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_MISSING_MONITOR_RECOVERY_API_VERSION ||
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
 * Provide the desktop missing monitor recovery set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_desktop_missing_monitor_recovery_set_subject(UmiDesktopMissingMonitorRecoverySnapshot *value, const char *subject_id)
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
 * Provide the desktop missing monitor recovery set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_desktop_missing_monitor_recovery_set_detail(UmiDesktopMissingMonitorRecoverySnapshot *value, const char *detail)
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
 * Provide the desktop missing monitor recovery set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_missing_monitor_recovery_set_state(UmiDesktopMissingMonitorRecoverySnapshot *value, uint32_t state)
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
 * Provide the desktop missing monitor recovery set priority operation used by this module
 * and its client applications.
 */
UmiStatus umi_desktop_missing_monitor_recovery_set_priority(UmiDesktopMissingMonitorRecoverySnapshot *value, uint32_t priority)
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
 * Provide the desktop missing monitor recovery set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_desktop_missing_monitor_recovery_set_enabled(UmiDesktopMissingMonitorRecoverySnapshot *value, bool enabled)
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
 * Provide the desktop missing monitor recovery same identity operation used by this module
 * and its client applications.
 */
bool umi_desktop_missing_monitor_recovery_same_identity(const UmiDesktopMissingMonitorRecoverySnapshot *left, const UmiDesktopMissingMonitorRecoverySnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop missing monitor recovery recover operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_missing_monitor_recovery_recover(const UmiDesktopControlRect *primary_work_area, UmiDesktopControlRect *window_geometry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (primary_work_area == NULL || window_geometry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_desktop_control_rect_clamp(window_geometry, primary_work_area);
}
