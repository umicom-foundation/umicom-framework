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

/* These public preview values are caller-owned. Validate before using restored
 * storage; counts are checked before traversal and unused rows are ignored.
 * Required inline text must be terminated and nonempty; currency remains an
 * opaque bounded label, not validated ISO membership. All amounts are finite.
 * Keep inputs stable on one owner thread. Output objects must not overlap inputs. */
UmiStatus umi_open_tms_cash_record_validate(const UmiOpenTmsCashRecord *record);
UmiStatus umi_open_tms_cash_book_validate(const UmiOpenTmsCashBook *book);
typedef struct UmiOpenTmsCashMeasures { double netForecast; double headroom; } UmiOpenTmsCashMeasures;
/* Complete finite result or unchanged output. Signed preview values are allowed.
 * This is double arithmetic, not an exact-money posting or an FX conversion. */
UmiStatus umi_open_tms_cash_record_measure(const UmiOpenTmsCashRecord *record, UmiOpenTmsCashMeasures *out);
/* A total requires one currency label across the book; mixed labels refuse.
 * An initialized empty book yields zero. Overflow preserves the output. */
UmiStatus umi_open_tms_cash_book_headroom_checked(const UmiOpenTmsCashBook *book, double *out);
/* Scalar compatibility functions return zero on any refusal. Use checked
 * results when zero must be distinguished from unavailable evidence. */

void umi_open_tms_cash_book_init(UmiOpenTmsCashBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
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
