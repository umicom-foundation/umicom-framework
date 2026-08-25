/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/panel_chrome.c
 *
 * PURPOSE:
 *   Implement semantic panel chrome state for titles, badges, pin/close actions and compact professional headers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/panel_chrome.h"

UmiStatus umi_ws_panel_chrome_init(UmiWsPanelChrome *chrome, const char *title) {
    UmiStatus status;
    if (chrome == NULL || title == NULL || title[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    *chrome = (UmiWsPanelChrome){0};
    status = umi_ws_copy_text(chrome->title, sizeof(chrome->title), title);
    if (status != UMI_STATUS_OK) return status;
    chrome->show_close = true;
    chrome->show_pin = true;
    chrome->show_menu = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_panel_chrome_set_badge(UmiWsPanelChrome *chrome, const char *badge) {
    if (chrome == NULL || badge == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(chrome->badge, sizeof(chrome->badge), badge);
}

void umi_ws_panel_chrome_set_compact(UmiWsPanelChrome *chrome, bool compact) {
    if (chrome != NULL) chrome->compact = compact;
}
