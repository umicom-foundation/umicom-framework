/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/panel_chrome.h
 *
 * PURPOSE:
 *   Define semantic panel chrome state for titles, badges, pin/close actions and compact professional headers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
} UmiWsPanelChrome;

UmiStatus umi_ws_panel_chrome_init(UmiWsPanelChrome *chrome, const char *title);
UmiStatus umi_ws_panel_chrome_set_badge(UmiWsPanelChrome *chrome, const char *badge);
void umi_ws_panel_chrome_set_compact(UmiWsPanelChrome *chrome, bool compact);

#ifdef __cplusplus
}
#endif

#endif
