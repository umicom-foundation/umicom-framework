/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_snapshot.c
 *
 * PURPOSE:
 *   Implement the test layout snapshot behavior for
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
#include "umicom/ui/workstation/layout_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l;
    UmiWsLayoutSnapshot s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_template_init(&l, "media.edit", "Edit", UMI_WS_DOMAIN_VIDEO_EDITOR) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_snapshot_capture(&s, "snap.1", &l, 1U, 1000, true) != UMI_STATUS_OK) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_ws_layout_snapshot_matches(&s, &l) || !s.autosave) return 3;
    puts("layout snapshot: ok");
    return 0;
}
