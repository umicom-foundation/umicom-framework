/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_trader_layouts.c
 *
 * PURPOSE:
 *   Implement the test trader layouts behavior for
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
#include "umicom/ui/workstation/trader_layouts.h"
int main(void) {
    UmiWsLayoutTemplate l;
    if (umi_ws_trader_layout_active(&l) != UMI_STATUS_OK || l.count != 5U) return 1;
    if (umi_ws_trader_layout_multi_chart(&l) != UMI_STATUS_OK || l.count != 5U) return 2;
    if (umi_ws_trader_layout_research(&l) != UMI_STATUS_OK || l.count != 4U) return 3;
    if (umi_ws_trader_layout_portfolio_risk(&l) != UMI_STATUS_OK || l.count != 4U) return 4;
    if (umi_ws_trader_layout_scalper(&l) != UMI_STATUS_OK || l.count != 4U) return 5;
    if (umi_ws_trader_layout_strategy_lab(&l) != UMI_STATUS_OK || l.count != 5U) return 6;
    if (umi_ws_trader_layout_options(&l) != UMI_STATUS_OK || l.count != 5U) return 7;
    puts("trader layouts: ok");
    return 0;
}
