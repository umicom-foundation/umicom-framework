/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_studio_layouts.c
 *
 * PURPOSE:
 *   Implement the test studio layouts behavior for
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
#include "umicom/ui/workstation/studio_layouts.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_welcome(&l) != UMI_STATUS_OK || l.count != 6U) return 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_code(&l) != UMI_STATUS_OK || l.count != 5U) return 2;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_debug(&l) != UMI_STATUS_OK || l.count != 5U) return 3;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_designer(&l) != UMI_STATUS_OK || l.count != 4U) return 4;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_diff_merge(&l) != UMI_STATUS_OK || l.count != 3U) return 5;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_testing(&l) != UMI_STATUS_OK || l.count != 4U) return 6;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_database(&l) != UMI_STATUS_OK || l.count != 4U) return 7;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_ai_development(&l) != UMI_STATUS_OK || l.count != 5U) return 8;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_ai_teacher(&l) != UMI_STATUS_OK || l.count != 5U) return 9;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_studio_layout_focus(&l) != UMI_STATUS_OK || l.count != 1U) return 10;
    puts("studio layouts: ok");
    return 0;
}
