/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_os_layouts.c
 *
 * PURPOSE:
 *   Implement the test os layouts behavior for
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
#include "umicom/ui/workstation/os_layouts.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_os_layout_desktop(&l) != UMI_STATUS_OK || l.count != 3U) return 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_os_layout_systems(&l) != UMI_STATUS_OK || l.count != 4U) return 2;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_os_layout_monitor(&l) != UMI_STATUS_OK || l.count != 4U) return 3;
    puts("os layouts: ok");
    return 0;
}
