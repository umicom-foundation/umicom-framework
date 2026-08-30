/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/application_health_summary.c
 * PURPOSE: Implement aggregate reusable application health state for Desk, Studio and other shells.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/application_health_summary.h"

#include <string.h>

void umi_desktop_application_health_summary_init(UmiDesktopApplicationHealthSummarySnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_APPLICATION_HEALTH_SUMMARY_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_application_health_summary_validate(const UmiDesktopApplicationHealthSummarySnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_APPLICATION_HEALTH_SUMMARY_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_health_summary_set_subject(UmiDesktopApplicationHealthSummarySnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_application_health_summary_set_detail(UmiDesktopApplicationHealthSummarySnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_application_health_summary_set_state(UmiDesktopApplicationHealthSummarySnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_health_summary_set_priority(UmiDesktopApplicationHealthSummarySnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_health_summary_set_enabled(UmiDesktopApplicationHealthSummarySnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_application_health_summary_same_identity(const UmiDesktopApplicationHealthSummarySnapshot *left, const UmiDesktopApplicationHealthSummarySnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

UmiDesktopControlSeverity umi_desktop_application_health_summary_severity(uint32_t failed_checks, uint32_t degraded_checks)
{
    if (failed_checks > 0U) return UMI_DESKTOP_CONTROL_SEVERITY_ERROR;
    if (degraded_checks > 0U) return UMI_DESKTOP_CONTROL_SEVERITY_WARNING;
    return UMI_DESKTOP_CONTROL_SEVERITY_NONE;
}
