/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_perspective_catalogue.c
 *
 * PURPOSE:
 *   Implement the test perspective catalogue behavior for
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
#include "umicom/ui/workstation/perspective_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsPerspectiveCatalogue c;
    UmiWsPerspectiveTemplate p;
    umi_ws_perspective_catalogue_init(&c);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_init(&p, "trader.research", "Research", UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_catalogue_upsert(&c, &p) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_perspective_catalogue_find(&c, "trader.research") == NULL) return 3;
    puts("perspective catalogue: ok");
    return 0;
}
