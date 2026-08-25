#include <stdio.h>
#include "umicom/ui/workstation/layout_edit_session.h"
int main(void) {
    UmiWsLayoutTemplate l,out;
    UmiWsLayoutEditSession s;
    if (umi_ws_layout_template_init(&l, "x", "X", UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK) return 1;
    if (umi_ws_layout_edit_session_begin(&s, &l) != UMI_STATUS_OK) return 2;
    if (umi_ws_layout_template_add(umi_ws_layout_edit_session_working(&s), "panel", UMI_WS_DOCK_LEFT, 0, 0, 1.0) != UMI_STATUS_OK) return 3;
    umi_ws_layout_edit_session_mark_dirty(&s);
    if (!s.dirty) return 4;
    if (umi_ws_layout_edit_session_commit(&s, &out) != UMI_STATUS_OK || out.count != 1U) return 5;
    puts("layout edit session: ok");
    return 0;
}
