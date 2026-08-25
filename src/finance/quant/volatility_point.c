/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/volatility_point.c
 *
 * PURPOSE:
 *   Represent one volatility observation by expiry and strike.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/volatility_point.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_volatility_point_init(UmiQuantVolatilityPoint *record, double expiry_years, double strike, double volatility)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(expiry_years >= 0.0 && strike >= 0.0 && volatility >= 0.0 && volatility < 10.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->expiry_years = expiry_years;
    record->strike = strike;
    record->volatility = volatility;
    return UMI_STATUS_OK;
}

/* Return total variance volatility squared times expiry. */
double umi_quant_volatility_point_variance(const UmiQuantVolatilityPoint *record)
{
    if (record == NULL) return 0.0;
    return record->volatility * record->volatility * record->expiry_years;
}
