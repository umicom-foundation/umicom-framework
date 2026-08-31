/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_panel_chrome.c
 *
 * PURPOSE:
 *   Implement the test panel chrome behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "umicom/ui/workstation/panel_chrome.h"
int main(void) {
    UmiWsPanelChrome c;
    if (umi_ws_panel_chrome_init(&c, "Project") != UMI_STATUS_OK) return 1;
    if (umi_ws_panel_chrome_set_badge(&c, "3") != UMI_STATUS_OK) return 2;
    if (umi_ws_panel_chrome_set_identity(&c, "project", "left") !=
        UMI_STATUS_OK) return 3;
    if (umi_ws_panel_chrome_set_context(&c, "project-blue", "blue") !=
        UMI_STATUS_OK) return 4;
    umi_ws_panel_chrome_set_compact(&c, true);
    if (!c.compact || c.badge[0] != '3' || !c.show_menu ||
        !c.show_context || !c.show_move || !c.show_float ||
        !c.show_maximise || !c.show_settings) return 5;
    if (strcmp(c.panel_id, "project") != 0 ||
        strcmp(c.context_group_id, "project-blue") != 0) return 6;
    if (strcmp(umi_ws_panel_action_text(UMI_WS_PANEL_ACTION_CLOSE),
               "Close panel") != 0) return 7;
    puts("panel chrome: ok");
    return 0;
}
