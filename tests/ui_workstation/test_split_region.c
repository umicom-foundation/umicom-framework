/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_split_region.c
 *
 * PURPOSE:
 *   Implement the test split region behavior for
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
#include "umicom/ui/workstation/split_region.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsSplitRegion s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_split_region_init(&s, "main.split", UMI_UI_HORIZONTAL, 0.7) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_split_region_bind(&s, "left", "centre") != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_split_region_set_ratio(&s, 0.99) != 0.85) return 3;
    puts("split region: ok");
    return 0;
}
