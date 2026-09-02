/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/money_market_loan.c
 *
 * PURPOSE:
 *   Represent a simple-interest money-market loan.
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

#include "umicom/finance/quant/money_market_loan.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_money_market_loan_init(UmiQuantMoneyMarketLoan *record, double principal, double rate, int32_t days, int32_t day_basis)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(umi_quant_number_valid(principal) && principal >= 0.0 && umi_quant_number_valid(rate) && days >= 0 && day_basis > 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->principal = principal;
    record->rate = rate;
    record->days = days;
    record->day_basis = day_basis;
    return UMI_STATUS_OK;
}

/* Calculate borrower interest using simple money-market accrual. */
double umi_quant_money_market_loan_interest(const UmiQuantMoneyMarketLoan *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->principal * record->rate * ((double)record->days / (double)record->day_basis);
}
