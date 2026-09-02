/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/panel_chrome.c
 *
 * PURPOSE:
 *   Implement semantic panel chrome state for titles, badges, pin/close actions and compact professional headers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/panel_chrome.h"

/*
 * Initialise ws panel chrome from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_panel_chrome_init(UmiWsPanelChrome *chrome, const char *title) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome == NULL || title == NULL || title[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    *chrome = (UmiWsPanelChrome){0};
    status = umi_ws_copy_text(chrome->title, sizeof(chrome->title), title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    chrome->show_close = true;
    chrome->show_pin = true;
    chrome->show_menu = true;
    chrome->show_context = true;
    chrome->show_move = true;
    chrome->show_float = true;
    chrome->show_maximise = true;
    chrome->show_settings = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws panel chrome set identity operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_panel_chrome_set_identity(
    UmiWsPanelChrome *chrome,
    const char *panel_id,
    const char *placement_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome == NULL || panel_id == NULL || panel_id[0] == '\0' ||
        placement_id == NULL || placement_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ws_copy_text(chrome->panel_id, sizeof(chrome->panel_id),
                              panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ws_copy_text(chrome->placement_id,
                                  sizeof(chrome->placement_id), placement_id);
    return status;
}

/*
 * Provide the ws panel chrome set context operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_panel_chrome_set_context(
    UmiWsPanelChrome *chrome,
    const char *group_id,
    const char *colour_token)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome == NULL || group_id == NULL || colour_token == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ws_copy_text(chrome->context_group_id,
                              sizeof(chrome->context_group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ws_copy_text(chrome->context_colour_token,
                                  sizeof(chrome->context_colour_token),
                                  colour_token);
    return status;
}

/*
 * Provide the ws panel chrome set badge operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_panel_chrome_set_badge(UmiWsPanelChrome *chrome, const char *badge) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome == NULL || badge == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(chrome->badge, sizeof(chrome->badge), badge);
}

/*
 * Provide the ws panel chrome set compact operation used by this module and its client
 * applications.
 */
void umi_ws_panel_chrome_set_compact(UmiWsPanelChrome *chrome, bool compact) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome != NULL) chrome->compact = compact;
}

/*
 * Provide the ws panel action text operation used by this module and its client
 * applications.
 */
const char *umi_ws_panel_action_text(UmiWsPanelAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
    case UMI_WS_PANEL_ACTION_PIN_TOGGLE: return "Pin or unpin panel";
    case UMI_WS_PANEL_ACTION_CONTEXT_GROUP: return "Choose linked context";
    case UMI_WS_PANEL_ACTION_MOVE: return "Move panel";
    case UMI_WS_PANEL_ACTION_FLOAT_TOGGLE: return "Dock or float panel";
    case UMI_WS_PANEL_ACTION_MAXIMISE_TOGGLE: return "Maximise or restore panel";
    case UMI_WS_PANEL_ACTION_SETTINGS: return "Panel settings";
    case UMI_WS_PANEL_ACTION_CLOSE: return "Close panel";
    default: return "Unknown panel action";
    }
}
