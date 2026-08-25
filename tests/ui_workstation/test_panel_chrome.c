#include <stdio.h>
#include "umicom/ui/workstation/panel_chrome.h"
int main(void) {
    UmiWsPanelChrome c;
    if (umi_ws_panel_chrome_init(&c, "Project") != UMI_STATUS_OK) return 1;
    if (umi_ws_panel_chrome_set_badge(&c, "3") != UMI_STATUS_OK) return 2;
    umi_ws_panel_chrome_set_compact(&c, true);
    if (!c.compact || c.badge[0] != '3' || !c.show_menu) return 3;
    puts("panel chrome: ok");
    return 0;
}
