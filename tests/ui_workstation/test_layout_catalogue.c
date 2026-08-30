/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_catalogue.c
 *
 * PURPOSE:
 *   Implement the test layout catalogue behavior for
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
#include "umicom/ui/workstation/layout_catalogue.h"
int main(void) {
    UmiWsLayoutCatalogue c;
    UmiWsLayoutTemplate l;
    umi_ws_layout_catalogue_init(&c);
    if (umi_ws_layout_template_init(&l, "trader.active", "Active Trading", UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK) return 1;
    if (umi_ws_layout_catalogue_upsert(&c, &l) != UMI_STATUS_OK) return 2;
    if (umi_ws_layout_catalogue_find(&c, "trader.active") == NULL) return 3;
    if (umi_ws_layout_catalogue_count_domain(&c, UMI_WS_DOMAIN_TRADER) != 1U) return 4;
    puts("layout catalogue: ok");
    return 0;
}
