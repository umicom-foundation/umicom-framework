/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_position_book.c
 *
 * PURPOSE:
 *   Verify treasury position versioning, exposure and unrealised P&L.
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
#include "umicom/open_tms/position_book.h"

int main(void)
{
    UmiOpenTmsPositionBook book;
    UmiOpenTmsPosition p={0};
    umi_open_tms_position_book_init(&book);
    (void)strcpy(p.accountId,"acct-1"); (void)strcpy(p.instrumentId,"GBPUSD");
    (void)strcpy(p.currency,"USD"); p.quantity=10.0; p.averagePrice=100.0;
    p.marketPrice=105.0; p.version=1U;
    assert(umi_open_tms_position_book_upsert(&book,&p)==UMI_STATUS_OK);
    assert(fabs(umi_open_tms_position_book_gross(&book)-1050.0)<0.001);
    assert(fabs(umi_open_tms_position_book_unrealised_pnl(&book)-50.0)<0.001);
    return 0;
}
