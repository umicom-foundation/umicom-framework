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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_trader_layout_active(&l) != UMI_STATUS_OK || l.count != 5U) return 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_trader_layout_multi_chart(&l) != UMI_STATUS_OK || l.count != 5U) return 2;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_trader_layout_research(&l) != UMI_STATUS_OK || l.count != 4U) return 3;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_trader_layout_portfolio_risk(&l) != UMI_STATUS_OK || l.count != 4U) return 4;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_trader_layout_scalper(&l) != UMI_STATUS_OK || l.count != 4U) return 5;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_trader_layout_strategy_lab(&l) != UMI_STATUS_OK || l.count != 5U) return 6;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_trader_layout_options(&l) != UMI_STATUS_OK || l.count != 5U) return 7;
    puts("trader layouts: ok");
    return 0;
}
