/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_settlement_book.c
 *
 * PURPOSE:
 *   Verify settlement lifecycle versioning and overdue controls.
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
#include "umicom/financial_platform/settlement_book.h"

int main(void)
{
    UmiFinancialSettlementBook book;
    UmiFinancialSettlementControl record = {0};
    umi_financial_settlement_book_init(&book);
    (void)strcpy(record.settlementId, "settle-1");
    (void)strcpy(record.tradeOrPaymentId, "trade-1");
    (void)strcpy(record.accountId, "acct-1");
    (void)strcpy(record.currency, "GBP");
    record.cashAmount = 100.0;
    record.dueMilliseconds = 2000U;
    record.version = 1U;
    assert(umi_financial_settlement_book_add(&book, &record) == UMI_STATUS_OK);
    assert(umi_financial_settlement_book_overdue_count(&book, 3000U) == 1U);
    assert(umi_financial_settlement_book_transition(
               &book, "settle-1", UMI_FINANCIAL_SETTLEMENT_SETTLED, 2U) ==
           UMI_STATUS_OK);
    assert(umi_financial_settlement_book_overdue_count(&book, 3000U) == 0U);
    return 0;
}
