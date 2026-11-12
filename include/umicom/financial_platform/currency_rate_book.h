/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/currency_rate_book.h
 *
 * PURPOSE:
 *   Retain bounded currency conversion rates with monotonic timestamps for
 *   cross-application financial controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_CURRENCY_RATE_BOOK_H
#define UMICOM_FINANCIAL_PLATFORM_CURRENCY_RATE_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_RATE_CAPACITY 128U

typedef struct UmiFinancialRateRecord {
    char base[8];
    char quote[8];
    double rate;
    uint64_t eventMilliseconds;
} UmiFinancialRateRecord;

typedef struct UmiFinancialRateBook {
    UmiFinancialRateRecord records[UMI_FINANCIAL_RATE_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialRateBook;

/* Public storage is caller-owned. Validate count before traversal, nonzero
 * revision, every bounded nonempty label, finite positive rate and unique pair.
 * Labels remain case-sensitive opaque strings, not verified ISO currency codes.
 * Equal timestamps may replace an observation; older timestamps refuse.
 * Serialize use; unused rows and bytes after string terminators are ignored. */
UmiStatus umi_financial_rate_record_validate(const UmiFinancialRateRecord *record);
UmiStatus umi_financial_rate_book_validate(const UmiFinancialRateBook *book);
/* Queries inspect at most eight bytes per label. Supply terminated readable
 * text or an eight-byte readable array. All outputs must be disjoint from input
 * storage and remain unchanged on refusal. Conversion is a double-valued preview:
 * no rounding, freshness policy, inverse lookup, triangulation or posting occurs.
 * Same-label conversion preserves a finite amount after validating the owner.
 * A non-finite product or nonzero amount underflowing to zero refuses with
 * CAPACITY_EXCEEDED; finite subnormal values remain supported. */
void umi_financial_rate_book_init(UmiFinancialRateBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_financial_rate_book_upsert(
    UmiFinancialRateBook *book,
    const UmiFinancialRateRecord *record);
UmiStatus umi_financial_rate_book_find(
    const UmiFinancialRateBook *book,
    const char *base,
    const char *quote,
    UmiFinancialRateRecord *outRecord);
UmiStatus umi_financial_rate_book_convert(
    const UmiFinancialRateBook *book,
    const char *base,
    const char *quote,
    double amount,
    double *outAmount);

#ifdef __cplusplus
}
#endif
#endif
