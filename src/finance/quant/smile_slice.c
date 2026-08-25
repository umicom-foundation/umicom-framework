/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/smile_slice.c
 *
 * PURPOSE:
 *   Represent simple strike-volatility smile slope evidence.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/smile_slice.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_smile_slice_init(UmiQuantSmileSlice *record, double lower_strike, double lower_vol, double upper_strike, double upper_vol)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(upper_strike > lower_strike && lower_vol >= 0.0 && upper_vol >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->lower_strike = lower_strike;
    record->lower_vol = lower_vol;
    record->upper_strike = upper_strike;
    record->upper_vol = upper_vol;
    return UMI_STATUS_OK;
}

/* Return linear volatility smile slope between two strikes. */
double umi_quant_smile_slice_slope(const UmiQuantSmileSlice *record)
{
    if (record == NULL) return 0.0;
    return (record->upper_vol - record->lower_vol) / (record->upper_strike - record->lower_strike);
}
