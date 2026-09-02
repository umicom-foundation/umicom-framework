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

/*
 * Initialise desktop window placement from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_window_placement_init(UmiDesktopWindowPlacementSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_WINDOW_PLACEMENT_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop window placement satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_window_placement_validate(const UmiDesktopWindowPlacementSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_WINDOW_PLACEMENT_API_VERSION ||
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
 * Provide the desktop window placement set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_window_placement_set_subject(UmiDesktopWindowPlacementSnapshot *value, const char *subject_id)
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
 * Provide the desktop window placement set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_window_placement_set_detail(UmiDesktopWindowPlacementSnapshot *value, const char *detail)
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
 * Provide the desktop window placement set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_window_placement_set_state(UmiDesktopWindowPlacementSnapshot *value, uint32_t state)
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
 * Provide the desktop window placement set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_window_placement_set_priority(UmiDesktopWindowPlacementSnapshot *value, uint32_t priority)
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
 * Provide the desktop window placement set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_window_placement_set_enabled(UmiDesktopWindowPlacementSnapshot *value, bool enabled)
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
 * Provide the desktop window placement same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_window_placement_same_identity(const UmiDesktopWindowPlacementSnapshot *left, const UmiDesktopWindowPlacementSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop window placement clamp operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_window_placement_clamp(UmiDesktopControlRect *geometry, const UmiDesktopControlRect *work_area)
{
    return umi_desktop_control_rect_clamp(geometry, work_area);
}
