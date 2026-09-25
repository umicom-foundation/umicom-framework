/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_book.c
 *
 * PURPOSE:
 *   Verify FX-forward lifecycle versioning and MTM evidence.
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
#include "umicom/open_tms/fx_forward_book.h"

int main(void)
{
    UmiOpenTmsFxForwardBook book;
    UmiOpenTmsFxForward trade = {0};
    umi_open_tms_fx_forward_book_init(&book);
    (void)strcpy(trade.tradeId, "fxf-1");
    (void)strcpy(trade.accountId, "treasury-1");
    (void)strcpy(trade.counterpartyId, "bank-1");
    (void)strcpy(trade.baseCurrency, "GBP");
    (void)strcpy(trade.quoteCurrency, "USD");
    trade.baseNotional = 1000000.0;
    trade.forwardRate = 1.25;
    trade.marketForwardRate = 1.26;
    trade.version = 1U;
    assert(umi_open_tms_fx_forward_book_add(&book, &trade) == UMI_STATUS_OK);
    assert(fabs(umi_open_tms_fx_forward_mark_to_market(&trade) - 10000.0) < 0.01);
    assert(umi_open_tms_fx_forward_book_transition(
               &book, "fxf-1", UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U) ==
           UMI_STATUS_OK);
    return 0;
}
