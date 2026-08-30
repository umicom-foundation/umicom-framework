/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/credit_spread.c
 *
 * PURPOSE:
 *   Represent credit spread over a benchmark yield.
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

#include "umicom/finance/quant/credit_spread.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_credit_spread_init(UmiQuantCreditSpread *record, double risky_yield, double benchmark_yield)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(risky_yield >= 0.0 && benchmark_yield >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->risky_yield = risky_yield;
    record->benchmark_yield = benchmark_yield;
    return UMI_STATUS_OK;
}

/* Return risky yield minus benchmark yield. */
double umi_quant_credit_spread_spread(const UmiQuantCreditSpread *record)
{
    if (record == NULL) return 0.0;
    return record->risky_yield - record->benchmark_yield;
}
