/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_cash_forecast.c
 *
 * PURPOSE:
 *   Exercise cash forecast validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_forecast.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryCashForecast v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_cash_forecast_init(&v, "fc-1", 1000, 900, 650) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_treasury_cash_forecast_net_minor(&v) != 250) return 2;
    return 0;
}
