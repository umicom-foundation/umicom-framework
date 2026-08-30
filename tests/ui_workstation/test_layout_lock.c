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
int main(void) {
    UmiWsLayoutLock l = umi_ws_layout_lock_default();
    if (umi_ws_layout_lock_allows(&l, UMI_WS_LAYOUT_MUTATION_MOVE)) return 1;
    if (!umi_ws_layout_lock_allows(&l, UMI_WS_LAYOUT_MUTATION_RESIZE)) return 2;
    umi_ws_layout_lock_set(&l, false);
    if (!umi_ws_layout_lock_allows(&l, UMI_WS_LAYOUT_MUTATION_MOVE)) return 3;
    puts("layout lock: ok");
    return 0;
}
