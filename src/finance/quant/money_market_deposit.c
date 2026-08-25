/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/money_market_deposit.c
 *
 * PURPOSE:
 *   Represent a simple-interest money-market deposit.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/money_market_deposit.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_money_market_deposit_init(UmiQuantMoneyMarketDeposit *record, double principal, double rate, int32_t days, int32_t day_basis)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(umi_quant_number_valid(principal) && principal >= 0.0 && umi_quant_number_valid(rate) && days >= 0 && day_basis > 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->principal = principal;
    record->rate = rate;
    record->days = days;
    record->day_basis = day_basis;
    return UMI_STATUS_OK;
}

/* Calculate simple accrued interest using the configured day basis. */
double umi_quant_money_market_deposit_interest(const UmiQuantMoneyMarketDeposit *record)
{
    if (record == NULL) return 0.0;
    return record->principal * record->rate * ((double)record->days / (double)record->day_basis);
}
