/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_inspector_model.c
 *
 * PURPOSE:
 *   Implement the test inspector model behavior for
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
#include "umicom/ui/workstation/inspector_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsInspectorModel m;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_inspector_model_init(&m, "selected.widget") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_inspector_model_set(&m, "width", "Width", "320", true) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_inspector_model_find(&m, "width") == NULL) return 3;
    puts("inspector model: ok");
    return 0;
}
