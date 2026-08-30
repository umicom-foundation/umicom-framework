/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_forecast.c
 *
 * PURPOSE:
 *   Implement aggregate expected cash inflows and outflows for a forecast horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_forecast.h"
#include <string.h>
UmiStatus umi_treasury_cash_forecast_init(UmiTreasuryCashForecast *value,
    const char *id,
    int64_t horizon_end_epoch_millis,
    int64_t inflow_minor,
    int64_t outflow_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->horizon_end_epoch_millis=horizon_end_epoch_millis;
    value->inflow_minor=inflow_minor;
    value->outflow_minor=outflow_minor;
    return umi_treasury_cash_forecast_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_cash_forecast_valid(const UmiTreasuryCashForecast *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->horizon_end_epoch_millis >= 0 && value->inflow_minor >= 0 && value->outflow_minor >= 0);
}

int64_t umi_treasury_cash_forecast_net_minor(const UmiTreasuryCashForecast *value) {
    if (value == NULL) return (int64_t)0;
    return value->inflow_minor - value->outflow_minor;
}
