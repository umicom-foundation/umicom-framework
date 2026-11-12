/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/position_control_book.h
 *
 * PURPOSE:
 *   Aggregate bounded position-control snapshots by account, instrument and
 *   currency without replacing the canonical finance position engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_POSITION_CONTROL_BOOK_H
#define UMICOM_FINANCIAL_PLATFORM_POSITION_CONTROL_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_POSITION_CONTROL_CAPACITY 512U

typedef struct UmiFinancialPositionControl {
    char accountId[64];
    char instrumentId[64];
    char currency[8];
    double quantity;
    double averagePrice;
    double marketPrice;
    double realisedPnl;
    uint64_t sourceVersion;
} UmiFinancialPositionControl;

typedef struct UmiFinancialPositionControlBook {
    UmiFinancialPositionControl positions[UMI_FINANCIAL_POSITION_CONTROL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialPositionControlBook;

/* Validate complete caller-owned storage before traversal: bounded terminated
 * text, required identities, finite numeric observations, known states/boolean
 * flags and unique keys. Count must fit the array and revision must be nonzero.
 * These references do not replace canonical domain engines or authenticate
 * permissions. Keep inputs stable on one owner thread and outputs disjoint.
 * Borrowed find results last until the next mutation; queries inspect at most
 * 64 readable bytes and must terminate. Unused rows/text tails are ignored. */
UmiStatus umi_financial_position_control_validate(const UmiFinancialPositionControl *row);
UmiStatus umi_financial_position_control_book_validate(const UmiFinancialPositionControlBook *owner);
/* Signed observations and equal source versions remain supported. */
/* Sum quantity * marketPrice for one account only, using one currency label.
 * Mixed labels within that account or a non-finite product/sum refuse without
 * changing output. No currency conversion or exact-money rounding is provided.
 * Other accounts are not added; a missing account yields zero. The scalar query
 * returns zero on refusal, so new reports should use the checked status. */
UmiStatus umi_financial_position_control_book_market_value_checked(
    const UmiFinancialPositionControlBook *book, const char *accountId, double *outValue);
void umi_financial_position_control_book_init(UmiFinancialPositionControlBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_financial_position_control_book_upsert(
    UmiFinancialPositionControlBook *book,
    const UmiFinancialPositionControl *position);
const UmiFinancialPositionControl *umi_financial_position_control_book_find(
    const UmiFinancialPositionControlBook *book,
    const char *accountId,
    const char *instrumentId);
double umi_financial_position_control_book_market_value(
    const UmiFinancialPositionControlBook *book,
    const char *accountId);

#ifdef __cplusplus
}
#endif
#endif
