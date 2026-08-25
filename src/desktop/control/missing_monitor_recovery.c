/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/missing_monitor_recovery.c
 * PURPOSE: Implement describe fallback behaviour when a saved monitor is no longer present.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/missing_monitor_recovery.h"

#include <string.h>

void umi_desktop_missing_monitor_recovery_init(UmiDesktopMissingMonitorRecoverySnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_MISSING_MONITOR_RECOVERY_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_missing_monitor_recovery_validate(const UmiDesktopMissingMonitorRecoverySnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_MISSING_MONITOR_RECOVERY_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_missing_monitor_recovery_set_subject(UmiDesktopMissingMonitorRecoverySnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_missing_monitor_recovery_set_detail(UmiDesktopMissingMonitorRecoverySnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_missing_monitor_recovery_set_state(UmiDesktopMissingMonitorRecoverySnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_missing_monitor_recovery_set_priority(UmiDesktopMissingMonitorRecoverySnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_missing_monitor_recovery_set_enabled(UmiDesktopMissingMonitorRecoverySnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_missing_monitor_recovery_same_identity(const UmiDesktopMissingMonitorRecoverySnapshot *left, const UmiDesktopMissingMonitorRecoverySnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

UmiStatus umi_desktop_missing_monitor_recovery_recover(const UmiDesktopControlRect *primary_work_area, UmiDesktopControlRect *window_geometry)
{
    if (primary_work_area == NULL || window_geometry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_desktop_control_rect_clamp(window_geometry, primary_work_area);
}
