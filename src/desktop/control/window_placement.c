/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/window_placement.c
 * PURPOSE: Implement represent toolkit-neutral window geometry and monitor affinity.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/window_placement.h"

#include <string.h>

void umi_desktop_window_placement_init(UmiDesktopWindowPlacementSnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_WINDOW_PLACEMENT_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_window_placement_validate(const UmiDesktopWindowPlacementSnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_WINDOW_PLACEMENT_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_window_placement_set_subject(UmiDesktopWindowPlacementSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_window_placement_set_detail(UmiDesktopWindowPlacementSnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_window_placement_set_state(UmiDesktopWindowPlacementSnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_window_placement_set_priority(UmiDesktopWindowPlacementSnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_window_placement_set_enabled(UmiDesktopWindowPlacementSnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_window_placement_same_identity(const UmiDesktopWindowPlacementSnapshot *left, const UmiDesktopWindowPlacementSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

UmiStatus umi_desktop_window_placement_clamp(UmiDesktopControlRect *geometry, const UmiDesktopControlRect *work_area)
{
    return umi_desktop_control_rect_clamp(geometry, work_area);
}
