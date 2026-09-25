/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/trade_control_book.h
 *
 * PURPOSE:
 *   Retain bounded trade-control references, lifecycle state and economics
 *   evidence without replacing the canonical trade registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_TRADE_CONTROL_BOOK_H
#define UMICOM_FINANCIAL_PLATFORM_TRADE_CONTROL_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_TRADE_CONTROL_CAPACITY 512U

typedef enum UmiFinancialTradeControlState {
    UMI_FINANCIAL_TRADE_DRAFT = 0,
    UMI_FINANCIAL_TRADE_CONFIRMED = 1,
    UMI_FINANCIAL_TRADE_SETTLEMENT_PENDING = 2,
    UMI_FINANCIAL_TRADE_SETTLED = 3,
    UMI_FINANCIAL_TRADE_CANCELLED = 4
} UmiFinancialTradeControlState;

typedef struct UmiFinancialTradeControl {
    char tradeId[64];
    char accountId[64];
    char counterpartyId[64];
    char instrumentId[64];
    char currency[8];
    double quantity;
    double price;
    double notional;
    UmiFinancialTradeControlState state;
    uint64_t version;
} UmiFinancialTradeControl;

typedef struct UmiFinancialTradeControlBook {
    UmiFinancialTradeControl trades[UMI_FINANCIAL_TRADE_CONTROL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialTradeControlBook;

void umi_financial_trade_control_book_init(UmiFinancialTradeControlBook *book);
UmiStatus umi_financial_trade_control_book_add(
    UmiFinancialTradeControlBook *book,
    const UmiFinancialTradeControl *trade);
UmiStatus umi_financial_trade_control_book_transition(
    UmiFinancialTradeControlBook *book,
    const char *tradeId,
    UmiFinancialTradeControlState state,
    uint64_t version);
const UmiFinancialTradeControl *umi_financial_trade_control_book_find(
    const UmiFinancialTradeControlBook *book,
    const char *tradeId);

#ifdef __cplusplus
}
#endif
#endif
