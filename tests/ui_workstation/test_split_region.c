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
int main(void) {
    UmiWsSplitRegion s;
    if (umi_ws_split_region_init(&s, "main.split", UMI_UI_HORIZONTAL, 0.7) != UMI_STATUS_OK) return 1;
    if (umi_ws_split_region_bind(&s, "left", "centre") != UMI_STATUS_OK) return 2;
    if (umi_ws_split_region_set_ratio(&s, 0.99) != 0.85) return 3;
    puts("split region: ok");
    return 0;
}
