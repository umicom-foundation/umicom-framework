/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_toolbar_model.c
 *
 * PURPOSE:
 *   Implement the test toolbar model behavior for
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
#include "umicom/ui/workstation/toolbar_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsToolbarModel t;
    umi_ws_toolbar_model_init(&t);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_toolbar_model_add(&t, "run", "Run", "run.start", UMI_WS_TOOLBAR_BUTTON) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_toolbar_model_set_active(&t, "run", true) != UMI_STATUS_OK) return 2;
    /* Apply this operation only while the related capability or state is available. */
    if (!t.items[0].active) return 3;
    puts("toolbar model: ok");
    return 0;
}
