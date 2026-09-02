/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_lock.c
 *
 * PURPOSE:
 *   Implement the test layout lock behavior for
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
#include "umicom/ui/workstation/layout_lock.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutLock l = umi_ws_layout_lock_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_lock_allows(&l, UMI_WS_LAYOUT_MUTATION_MOVE)) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_layout_lock_allows(&l, UMI_WS_LAYOUT_MUTATION_RESIZE)) return 2;
    umi_ws_layout_lock_set(&l, false);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_layout_lock_allows(&l, UMI_WS_LAYOUT_MUTATION_MOVE)) return 3;
    puts("layout lock: ok");
    return 0;
}
