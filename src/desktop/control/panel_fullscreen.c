/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/panel_fullscreen.c
 * PURPOSE: Implement describe full-screen panel intent and restoration metadata.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/panel_fullscreen.h"

#include <string.h>

/*
 * Initialise desktop panel fullscreen from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_panel_fullscreen_init(UmiDesktopPanelFullscreenSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_PANEL_FULLSCREEN_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop panel fullscreen satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_panel_fullscreen_validate(const UmiDesktopPanelFullscreenSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_PANEL_FULLSCREEN_API_VERSION ||
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
 * Provide the desktop panel fullscreen set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_fullscreen_set_subject(UmiDesktopPanelFullscreenSnapshot *value, const char *subject_id)
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
 * Provide the desktop panel fullscreen set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_fullscreen_set_detail(UmiDesktopPanelFullscreenSnapshot *value, const char *detail)
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
 * Provide the desktop panel fullscreen set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_fullscreen_set_state(UmiDesktopPanelFullscreenSnapshot *value, uint32_t state)
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
 * Provide the desktop panel fullscreen set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_fullscreen_set_priority(UmiDesktopPanelFullscreenSnapshot *value, uint32_t priority)
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
 * Provide the desktop panel fullscreen set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_panel_fullscreen_set_enabled(UmiDesktopPanelFullscreenSnapshot *value, bool enabled)
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
 * Provide the desktop panel fullscreen same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_panel_fullscreen_same_identity(const UmiDesktopPanelFullscreenSnapshot *left, const UmiDesktopPanelFullscreenSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}
