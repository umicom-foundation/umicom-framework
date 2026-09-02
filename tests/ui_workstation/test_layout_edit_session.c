/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_edit_session.c
 *
 * PURPOSE:
 *   Implement the test layout edit session behavior for
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
#include "umicom/ui/workstation/layout_edit_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l,out;
    UmiWsLayoutEditSession s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_template_init(&l, "x", "X", UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_edit_session_begin(&s, &l) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_template_add(umi_ws_layout_edit_session_working(&s), "panel", UMI_WS_DOCK_LEFT, 0, 0, 1.0) != UMI_STATUS_OK) return 3;
    umi_ws_layout_edit_session_mark_dirty(&s);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!s.dirty) return 4;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_layout_edit_session_commit(&s, &out) != UMI_STATUS_OK || out.count != 1U) return 5;
    puts("layout edit session: ok");
    return 0;
}
