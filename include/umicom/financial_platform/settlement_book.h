/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/settlement_book.h
 *
 * PURPOSE:
 *   Retain settlement-control records and explicit due/fail/completion states
 *   without replacing canonical settlement services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_BOOK_H
#define UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_SETTLEMENT_CAPACITY 512U

typedef enum UmiFinancialSettlementState {
    UMI_FINANCIAL_SETTLEMENT_PENDING = 0,
    UMI_FINANCIAL_SETTLEMENT_READY = 1,
    UMI_FINANCIAL_SETTLEMENT_SENT = 2,
    UMI_FINANCIAL_SETTLEMENT_SETTLED = 3,
    UMI_FINANCIAL_SETTLEMENT_FAILED = 4
} UmiFinancialSettlementState;

typedef struct UmiFinancialSettlementControl {
    char settlementId[64];
    char tradeOrPaymentId[64];
    char accountId[64];
    char currency[8];
    double cashAmount;
    double securitiesQuantity;
    uint64_t dueMilliseconds;
    UmiFinancialSettlementState state;
    uint64_t version;
} UmiFinancialSettlementControl;

typedef struct UmiFinancialSettlementBook {
    UmiFinancialSettlementControl records[UMI_FINANCIAL_SETTLEMENT_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialSettlementBook;

void umi_financial_settlement_book_init(UmiFinancialSettlementBook *book);
UmiStatus umi_financial_settlement_book_add(
    UmiFinancialSettlementBook *book,
    const UmiFinancialSettlementControl *record);
UmiStatus umi_financial_settlement_book_transition(
    UmiFinancialSettlementBook *book,
    const char *settlementId,
    UmiFinancialSettlementState state,
    uint64_t version);
const UmiFinancialSettlementControl *umi_financial_settlement_book_find(
    const UmiFinancialSettlementBook *book,
    const char *settlementId);
size_t umi_financial_settlement_book_overdue_count(
    const UmiFinancialSettlementBook *book,
    uint64_t nowMilliseconds);

#ifdef __cplusplus
}
#endif
#endif
