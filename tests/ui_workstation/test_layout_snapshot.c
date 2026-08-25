#include <stdio.h>
#include "umicom/ui/workstation/layout_snapshot.h"
int main(void) {
    UmiWsLayoutTemplate l;
    UmiWsLayoutSnapshot s;
    if (umi_ws_layout_template_init(&l, "media.edit", "Edit", UMI_WS_DOMAIN_VIDEO_EDITOR) != UMI_STATUS_OK) return 1;
    if (umi_ws_layout_snapshot_capture(&s, "snap.1", &l, 1U, 1000, true) != UMI_STATUS_OK) return 2;
    if (!umi_ws_layout_snapshot_matches(&s, &l) || !s.autosave) return 3;
    puts("layout snapshot: ok");
    return 0;
}
