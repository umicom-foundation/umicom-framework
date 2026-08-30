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
int main(void) {
    UmiWsWorkspaceStrip s;
    umi_ws_workspace_strip_init(&s);
    if (umi_ws_workspace_strip_add(&s, "code", "Code", "studio.code") != UMI_STATUS_OK) return 1;
    if (umi_ws_workspace_strip_add(&s, "debug", "Debug", "studio.debug") != UMI_STATUS_OK) return 2;
    if (umi_ws_workspace_strip_activate(&s, "debug") != UMI_STATUS_OK) return 3;
    if (strcmp(umi_ws_workspace_strip_active(&s)->layout_id, "studio.debug") != 0) return 4;
    puts("workspace strip: ok");
    return 0;
}
