/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_trade_control_book.c
 *
 * PURPOSE:
 *   Verify trade identity, lifecycle versioning and terminal-state protection.
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
#include "umicom/financial_platform/trade_control_book.h"

int main(void)
{
    UmiFinancialTradeControlBook book;
    UmiFinancialTradeControl trade = {0};
    umi_financial_trade_control_book_init(&book);
    (void)strcpy(trade.tradeId, "trade-1");
    (void)strcpy(trade.accountId, "acct-1");
    (void)strcpy(trade.counterpartyId, "cp-1");
    (void)strcpy(trade.instrumentId, "GBPUSD");
    (void)strcpy(trade.currency, "USD");
    trade.quantity = 100000.0;
    trade.price = 1.25;
    trade.notional = 125000.0;
    trade.version = 1U;
    assert(umi_financial_trade_control_book_add(&book, &trade) == UMI_STATUS_OK);
    assert(umi_financial_trade_control_book_transition(
               &book, "trade-1", UMI_FINANCIAL_TRADE_CONFIRMED, 2U) ==
           UMI_STATUS_OK);
    assert(umi_financial_trade_control_book_transition(
               &book, "trade-1", UMI_FINANCIAL_TRADE_SETTLED, 3U) ==
           UMI_STATUS_OK);
    assert(umi_financial_trade_control_book_transition(
               &book, "trade-1", UMI_FINANCIAL_TRADE_CANCELLED, 4U) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
