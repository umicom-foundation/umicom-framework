/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/notification_centre.c
 * PURPOSE: Implement represent notification-centre item metadata and read/priority state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/notification_centre.h"

#include <string.h>

/*
 * Initialise desktop notification centre from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_notification_centre_init(UmiDesktopNotificationCentreSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_NOTIFICATION_CENTRE_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop notification centre satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_desktop_notification_centre_validate(const UmiDesktopNotificationCentreSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_NOTIFICATION_CENTRE_API_VERSION ||
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
 * Provide the desktop notification centre set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_notification_centre_set_subject(UmiDesktopNotificationCentreSnapshot *value, const char *subject_id)
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
 * Provide the desktop notification centre set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_notification_centre_set_detail(UmiDesktopNotificationCentreSnapshot *value, const char *detail)
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
 * Provide the desktop notification centre set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_notification_centre_set_state(UmiDesktopNotificationCentreSnapshot *value, uint32_t state)
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
 * Provide the desktop notification centre set priority operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_notification_centre_set_priority(UmiDesktopNotificationCentreSnapshot *value, uint32_t priority)
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
 * Provide the desktop notification centre set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_notification_centre_set_enabled(UmiDesktopNotificationCentreSnapshot *value, bool enabled)
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
 * Provide the desktop notification centre same identity operation used by this module and
 * its client applications.
 */
bool umi_desktop_notification_centre_same_identity(const UmiDesktopNotificationCentreSnapshot *left, const UmiDesktopNotificationCentreSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop notification centre visible operation used by this module and its
 * client applications.
 */
bool umi_desktop_notification_centre_visible(UmiDesktopControlSeverity severity, UmiDesktopControlSeverity minimum_severity, bool dismissed)
{
    return !dismissed && severity >= minimum_severity;
}
