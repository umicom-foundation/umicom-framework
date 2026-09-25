/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_cash_book.c
 *
 * PURPOSE:
 *   Verify cash snapshot freshness, forecast and headroom calculations.
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
#include "umicom/open_tms/cash_book.h"

int main(void)
{
    UmiOpenTmsCashBook book;
    UmiOpenTmsCashRecord r = {0};
    umi_open_tms_cash_book_init(&book);
    (void)strcpy(r.accountId, "acct-1");
    (void)strcpy(r.currency, "GBP");
    r.projected = 1000.0;
    r.inflows = 300.0;
    r.outflows = 200.0;
    r.minimum = 500.0;
    r.eventMilliseconds = 1000U;
    assert(umi_open_tms_cash_book_upsert(&book, &r) == UMI_STATUS_OK);
    assert(fabs(umi_open_tms_cash_record_net_forecast(&r) - 1100.0) < 0.001);
    assert(fabs(umi_open_tms_cash_record_headroom(&r) - 600.0) < 0.001);
    return 0;
}
