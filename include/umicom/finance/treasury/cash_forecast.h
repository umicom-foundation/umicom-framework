/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_forecast.h
 *
 * PURPOSE:
 *   Aggregate expected cash inflows and outflows for a forecast horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_FORECAST_H
#define UMICOM_FINANCE_TREASURY_CASH_FORECAST_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCashForecast {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t horizon_end_epoch_millis;
    int64_t inflow_minor;
    int64_t outflow_minor;
} UmiTreasuryCashForecast;
UmiStatus umi_treasury_cash_forecast_init(UmiTreasuryCashForecast *value,
    const char *id,
    int64_t horizon_end_epoch_millis,
    int64_t inflow_minor,
    int64_t outflow_minor);
bool umi_treasury_cash_forecast_valid(const UmiTreasuryCashForecast *value);
int64_t umi_treasury_cash_forecast_net_minor(const UmiTreasuryCashForecast *value);
#ifdef __cplusplus
}
#endif
#endif
