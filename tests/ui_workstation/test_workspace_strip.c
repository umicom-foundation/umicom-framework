/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_strip.c
 *
 * PURPOSE:
 *   Implement the test workspace strip behavior for
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
#include <string.h>
#include "umicom/ui/workstation/workspace_strip.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsWorkspaceStrip s;
    umi_ws_workspace_strip_init(&s);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_workspace_strip_add(&s, "code", "Code", "studio.code") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_workspace_strip_add(&s, "debug", "Debug", "studio.debug") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_workspace_strip_activate(&s, "debug") != UMI_STATUS_OK) return 3;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_ws_workspace_strip_active(&s)->layout_id, "studio.debug") != 0) return 4;
    puts("workspace strip: ok");
    return 0;
}
