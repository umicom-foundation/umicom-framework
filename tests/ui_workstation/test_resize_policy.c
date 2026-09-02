/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_resize_policy.c
 *
 * PURPOSE:
 *   Implement the test resize policy behavior for
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
#include "umicom/ui/workstation/resize_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsResizePolicy p;
    UmiUiSize min = {100,80}, pref = {300,200}, max = {800,600}, req = {50,900};
    UmiUiSize out;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_resize_policy_init(&p, min, pref, max) != UMI_STATUS_OK) return 1;
    out = umi_ws_resize_policy_apply(&p, req);
    /* Apply this branch only when its contract condition is satisfied. */
    if (out.width != 100 || out.height != 600) return 2;
    puts("resize policy: ok");
    return 0;
}
