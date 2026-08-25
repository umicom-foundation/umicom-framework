/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/interest_rate_swap.c
 *
 * PURPOSE:
 *   Represent fixed-versus-floating swap economics.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/interest_rate_swap.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_interest_rate_swap_init(UmiQuantInterestRateSwap *record, double notional, double fixed_rate, double floating_rate, double year_fraction)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(notional >= 0.0 && year_fraction >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->notional = notional;
    record->fixed_rate = fixed_rate;
    record->floating_rate = floating_rate;
    record->year_fraction = year_fraction;
    return UMI_STATUS_OK;
}

/* Return receive-floating/pay-fixed period value. */
double umi_quant_interest_rate_swap_period_value(const UmiQuantInterestRateSwap *record)
{
    if (record == NULL) return 0.0;
    return record->notional * (record->floating_rate - record->fixed_rate) * record->year_fraction;
}
