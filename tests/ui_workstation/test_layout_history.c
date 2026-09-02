/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_history.c
 *
 * PURPOSE:
 *   Implement the test layout history behavior for
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
#include "umicom/ui/workstation/layout_history.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutHistory h;
    UmiWsLayoutTemplate l;
    UmiWsLayoutSnapshot a,b;
    umi_ws_layout_history_init(&h);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_template_init(&l, "layout", "Layout", UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_snapshot_capture(&a, "a", &l, 1U, 1, false) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_snapshot_capture(&b, "b", &l, 2U, 2, false) != UMI_STATUS_OK) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_history_push(&h, &a) != UMI_STATUS_OK || umi_ws_layout_history_push(&h, &b) != UMI_STATUS_OK) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_history_back(&h)->sequence != 1U) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_history_forward(&h)->sequence != 2U) return 6;
    puts("layout history: ok");
    return 0;
}
