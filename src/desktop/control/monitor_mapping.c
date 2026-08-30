/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/monitor_mapping.c
 * PURPOSE: Implement describe persisted-to-current monitor mapping decisions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/monitor_mapping.h"

#include <string.h>

void umi_desktop_monitor_mapping_init(UmiDesktopMonitorMappingSnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_MONITOR_MAPPING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_monitor_mapping_validate(const UmiDesktopMonitorMappingSnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_MONITOR_MAPPING_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_mapping_set_subject(UmiDesktopMonitorMappingSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_monitor_mapping_set_detail(UmiDesktopMonitorMappingSnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_monitor_mapping_set_state(UmiDesktopMonitorMappingSnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_mapping_set_priority(UmiDesktopMonitorMappingSnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_monitor_mapping_set_enabled(UmiDesktopMonitorMappingSnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_monitor_mapping_same_identity(const UmiDesktopMonitorMappingSnapshot *left, const UmiDesktopMonitorMappingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

uint32_t umi_desktop_monitor_mapping_confidence(bool exact_identity, bool same_geometry, bool primary_fallback)
{
    if (exact_identity) return 100U;
    if (same_geometry) return 75U;
    if (primary_fallback) return 25U;
    return 0U;
}
