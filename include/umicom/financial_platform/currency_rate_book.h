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

void umi_financial_rate_book_init(UmiFinancialRateBook *book);
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
