/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_currency_rate_book.c
 *
 * PURPOSE:
 *   Verify monotonic FX rates and deterministic conversion.
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
#include "umicom/financial_platform/currency_rate_book.h"

int main(void)
{
    UmiFinancialRateBook book;
    UmiFinancialRateRecord rate = {0};
    double converted = 0.0;

    umi_financial_rate_book_init(&book);
    (void)strcpy(rate.base, "GBP");
    (void)strcpy(rate.quote, "USD");
    rate.rate = 1.25;
    rate.eventMilliseconds = 1000U;
    assert(umi_financial_rate_book_upsert(&book, &rate) == UMI_STATUS_OK);
    assert(umi_financial_rate_book_convert(&book, "GBP", "USD", 100.0,
                                           &converted) == UMI_STATUS_OK);
    assert(fabs(converted - 125.0) < 0.001);
    rate.eventMilliseconds = 900U;
    assert(umi_financial_rate_book_upsert(&book, &rate) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
