/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_settlement_book.c
 *
 * PURPOSE:
 *   Verify treasury settlement failure, overdue and lifecycle controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/open_tms/settlement_book.h"
int main(void)
{
    UmiOpenTmsSettlementBook book;UmiOpenTmsSettlementRecord r={0};
    umi_open_tms_settlement_book_init(&book);
    (void)strcpy(r.id,"settle-1");(void)strcpy(r.sourceId,"fxf-1");(void)strcpy(r.accountId,"acct-1");(void)strcpy(r.currency,"USD");
    r.amount=100.0;r.dueMilliseconds=1000U;r.version=1U;
    assert(umi_open_tms_settlement_book_add(&book,&r)==UMI_STATUS_OK);
    assert(umi_open_tms_settlement_book_overdue_count(&book,2000U)==1U);
    assert(umi_open_tms_settlement_book_transition(&book,"settle-1",UMI_OPEN_TMS_SETTLEMENT_SETTLED,2U)==UMI_STATUS_OK);
    assert(umi_open_tms_settlement_book_overdue_count(&book,2000U)==0U);
    return 0;
}
