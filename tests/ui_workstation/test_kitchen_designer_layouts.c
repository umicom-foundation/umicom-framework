/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_kitchen_designer_layouts.c
 *
 * PURPOSE:
 *   Implement the test kitchen designer layouts behavior for
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
#include "umicom/ui/workstation/kitchen_designer_layouts.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_kitchen_layout_design(&l) != UMI_STATUS_OK || l.count != 4U) return 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_kitchen_layout_review(&l) != UMI_STATUS_OK || l.count != 4U) return 2;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_kitchen_layout_catalogue(&l) != UMI_STATUS_OK || l.count != 3U) return 3;
    puts("kitchen layouts: ok");
    return 0;
}
