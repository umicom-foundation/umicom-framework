/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/fra.c
 *
 * PURPOSE:
 *   Represent a forward-rate agreement payoff contract.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/fra.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_fra_init(UmiQuantFra *record, double notional, double fixed_rate, double forward_rate, int32_t days, int32_t day_basis)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(notional >= 0.0 && umi_quant_number_valid(fixed_rate) && umi_quant_number_valid(forward_rate) && days > 0 && day_basis > 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->notional = notional;
    record->fixed_rate = fixed_rate;
    record->forward_rate = forward_rate;
    record->days = days;
    record->day_basis = day_basis;
    return UMI_STATUS_OK;
}

/* Calculate the undiscounted FRA rate-difference payoff. */
double umi_quant_fra_undiscounted_payoff(const UmiQuantFra *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->notional * (record->forward_rate - record->fixed_rate) * ((double)record->days / (double)record->day_basis);
}
