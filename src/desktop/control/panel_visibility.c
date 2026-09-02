/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/panel_visibility.c
 * PURPOSE: Implement represent panel visibility intent separate from widget lifetime.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/panel_visibility.h"

#include <string.h>

/*
 * Initialise desktop panel visibility from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_panel_visibility_init(UmiDesktopPanelVisibilitySnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_PANEL_VISIBILITY_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop panel visibility satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_panel_visibility_validate(const UmiDesktopPanelVisibilitySnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_PANEL_VISIBILITY_API_VERSION ||
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
 * Provide the desktop panel visibility set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_visibility_set_subject(UmiDesktopPanelVisibilitySnapshot *value, const char *subject_id)
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
 * Provide the desktop panel visibility set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_visibility_set_detail(UmiDesktopPanelVisibilitySnapshot *value, const char *detail)
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
 * Provide the desktop panel visibility set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_visibility_set_state(UmiDesktopPanelVisibilitySnapshot *value, uint32_t state)
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
 * Provide the desktop panel visibility set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_visibility_set_priority(UmiDesktopPanelVisibilitySnapshot *value, uint32_t priority)
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
 * Provide the desktop panel visibility set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_visibility_set_enabled(UmiDesktopPanelVisibilitySnapshot *value, bool enabled)
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
 * Provide the desktop panel visibility same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_panel_visibility_same_identity(const UmiDesktopPanelVisibilitySnapshot *left, const UmiDesktopPanelVisibilitySnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop panel visibility effective operation used by this module and its
 * client applications.
 */
bool umi_desktop_panel_visibility_effective(bool requested_visible, bool provider_available)
{
    return requested_visible && provider_available;
}
