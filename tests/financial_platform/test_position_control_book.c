/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_position_control_book.c
 *
 * PURPOSE:
 *   Verify monotonic position snapshots and account market-value aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <string.h>
#include "umicom/financial_platform/position_control_book.h"

int main(void)
{
    UmiFinancialPositionControlBook book;
    UmiFinancialPositionControl p = {0};
    umi_financial_position_control_book_init(&book);
    (void)strcpy(p.accountId, "acct-1");
    (void)strcpy(p.instrumentId, "bond-1");
    (void)strcpy(p.currency, "GBP");
    p.quantity = 10.0;
    p.marketPrice = 101.0;
    p.sourceVersion = 2U;
    assert(umi_financial_position_control_book_upsert(&book, &p) == UMI_STATUS_OK);
    assert(fabs(umi_financial_position_control_book_market_value(
                    &book, "acct-1") - 1010.0) < 0.001);
    p.sourceVersion = 1U;
    assert(umi_financial_position_control_book_upsert(&book, &p) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
