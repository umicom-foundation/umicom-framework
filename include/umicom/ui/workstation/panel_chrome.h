/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/panel_chrome.h
 *
 * PURPOSE:
 *   Define semantic panel chrome state for titles, badges, pin/close actions and compact professional headers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_PANEL_CHROME_H
#define UMICOM_UI_WORKSTATION_PANEL_CHROME_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsPanelChrome {
    char title[UMI_UI_TEXT_CAPACITY];
    char subtitle[UMI_UI_TEXT_CAPACITY];
    char badge[UMI_UI_TEXT_CAPACITY];
    bool show_close;
    bool show_pin;
    bool show_menu;
    bool compact;
    char panel_id[UMI_UI_TEXT_CAPACITY];
    char placement_id[UMI_UI_TEXT_CAPACITY];
    char context_group_id[UMI_UI_TEXT_CAPACITY];
    char context_colour_token[UMI_UI_TEXT_CAPACITY];
    bool show_context;
    bool show_move;
    bool show_float;
    bool show_maximise;
    bool show_settings;
    bool pinned;
    bool locked;
    bool floating;
    bool maximised;
} UmiWsPanelChrome;

typedef enum UmiWsPanelAction {
    UMI_WS_PANEL_ACTION_PIN_TOGGLE = 1,
    UMI_WS_PANEL_ACTION_CONTEXT_GROUP = 2,
    UMI_WS_PANEL_ACTION_MOVE = 3,
    UMI_WS_PANEL_ACTION_FLOAT_TOGGLE = 4,
    UMI_WS_PANEL_ACTION_MAXIMISE_TOGGLE = 5,
    UMI_WS_PANEL_ACTION_SETTINGS = 6,
    UMI_WS_PANEL_ACTION_CLOSE = 7
} UmiWsPanelAction;

UmiStatus umi_ws_panel_chrome_init(UmiWsPanelChrome *chrome, const char *title);
UmiStatus umi_ws_panel_chrome_set_badge(UmiWsPanelChrome *chrome, const char *badge);
UmiStatus umi_ws_panel_chrome_set_identity(
    UmiWsPanelChrome *chrome,
    const char *panel_id,
    const char *placement_id);
UmiStatus umi_ws_panel_chrome_set_context(
    UmiWsPanelChrome *chrome,
    const char *group_id,
    const char *colour_token);
void umi_ws_panel_chrome_set_compact(UmiWsPanelChrome *chrome, bool compact);
const char *umi_ws_panel_action_text(UmiWsPanelAction action);

#ifdef __cplusplus
}
#endif

#endif
