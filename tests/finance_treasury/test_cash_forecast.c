/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_cash_forecast.c
 *
 * PURPOSE:
 *   Exercise cash forecast validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_forecast.h"
int main(void) {
    UmiTreasuryCashForecast v;
    if (umi_treasury_cash_forecast_init(&v, "fc-1", 1000, 900, 650) != UMI_STATUS_OK) return 1;
    if (umi_treasury_cash_forecast_net_minor(&v) != 250) return 2;
    return 0;
}
