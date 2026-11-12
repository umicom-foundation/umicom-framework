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

/* Validate complete caller-owned storage before traversal: bounded terminated
 * text, required identities, finite numeric observations, known states/boolean
 * flags and unique keys. Count must fit the array and revision must be nonzero.
 * These references do not replace canonical domain engines or authenticate
 * permissions. Keep inputs stable on one owner thread and outputs disjoint.
 * Borrowed find results last until the next mutation; queries inspect at most
 * 64 readable bytes and must terminate. Unused rows/text tails are ignored. */
UmiStatus umi_financial_settlement_control_validate(const UmiFinancialSettlementControl *row);
UmiStatus umi_financial_settlement_book_validate(const UmiFinancialSettlementBook *owner);
/* Imported observations may use any known state. Transition retains the
 * existing known-state edges, terminal rules and strictly increasing versions.
 * Updating these local values does not perform a financial transaction. */
/* A nonzero due time strictly before now is overdue unless state is SETTLED.
 * Publish one complete count or preserve output on refusal. The scalar query
 * returns zero on refusal; use this status to distinguish unavailable evidence. */
UmiStatus umi_financial_settlement_book_overdue_count_checked(
    const UmiFinancialSettlementBook *book, uint64_t nowMilliseconds, size_t *outCount);
void umi_financial_settlement_book_init(UmiFinancialSettlementBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_financial_settlement_book_add(
    UmiFinancialSettlementBook *book,
    const UmiFinancialSettlementControl *record);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
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
