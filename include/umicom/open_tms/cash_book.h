/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/cash_book.h
 *
 * PURPOSE:
 *   Aggregate settled, projected, minimum and forecast treasury cash by account/currency.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_CASH_BOOK_H
#define UMICOM_OPEN_TMS_CASH_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_CASH_CAPACITY 256U

typedef struct UmiOpenTmsCashRecord {
    char accountId[64];
    char currency[8];
    double settled;
    double projected;
    double minimum;
    double inflows;
    double outflows;
    uint64_t eventMilliseconds;
} UmiOpenTmsCashRecord;

typedef struct UmiOpenTmsCashBook {
    UmiOpenTmsCashRecord records[UMI_OPEN_TMS_CASH_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiOpenTmsCashBook;

void umi_open_tms_cash_book_init(UmiOpenTmsCashBook *book);
UmiStatus umi_open_tms_cash_book_upsert(
    UmiOpenTmsCashBook *book,
    const UmiOpenTmsCashRecord *record);
const UmiOpenTmsCashRecord *umi_open_tms_cash_book_find(
    const UmiOpenTmsCashBook *book,
    const char *accountId,
    const char *currency);
double umi_open_tms_cash_record_net_forecast(const UmiOpenTmsCashRecord *record);
double umi_open_tms_cash_record_headroom(const UmiOpenTmsCashRecord *record);

#ifdef __cplusplus
}
#endif
#endif
