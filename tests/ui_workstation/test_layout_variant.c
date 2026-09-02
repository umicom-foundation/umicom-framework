/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_variant.c
 *
 * PURPOSE:
 *   Implement the test layout variant behavior for
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
#include "umicom/ui/workstation/layout_variant.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutEnvironment e = {3440,1440,1U,100};
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_variant_select(&e) != UMI_WS_LAYOUT_ULTRAWIDE) return 1;
    e.monitor_count = 3U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_variant_select(&e) != UMI_WS_LAYOUT_TRIPLE_MONITOR) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_variant_text(UMI_WS_LAYOUT_LAPTOP)[0] != 'l') return 3;
    puts("layout variant: ok");
    return 0;
}
