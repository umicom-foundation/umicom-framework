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
/**
 * Represent the treasury cash forecast data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCashForecast {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t horizon_end_epoch_millis;
    int64_t inflow_minor;
    int64_t outflow_minor;
} UmiTreasuryCashForecast;
/**
 * Initialise treasury cash forecast from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_forecast_init(UmiTreasuryCashForecast *value,
    const char *id,
    int64_t horizon_end_epoch_millis,
    int64_t inflow_minor,
    int64_t outflow_minor);
/**
 * Check that treasury cash forecast satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_forecast_valid(const UmiTreasuryCashForecast *value);
/**
 * Provide the treasury cash forecast net minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_cash_forecast_net_minor(const UmiTreasuryCashForecast *value);
#ifdef __cplusplus
}
#endif
#endif
