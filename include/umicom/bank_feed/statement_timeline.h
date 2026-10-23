/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/statement_timeline.h
 * PURPOSE: Inspect daily booked movements without treating pending observations as balances.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_STATEMENT_TIMELINE_H
#define UMICOM_BANK_FEED_STATEMENT_TIMELINE_H
#include "umicom/bank_feed/statement.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBankStatementTimeline UmiBankStatementTimeline;
typedef struct UmiBankStatementDay {
    UmiFinancialDate date;
    UmiDecimal credits, debits, netMovement, closing;
    size_t bookedRows, pendingRows, mutableBookedRows, unidentifiedRows;
} UmiBankStatementDay;
typedef struct UmiBankStatementTimelineSummary {
    UmiBankStatementResult review;
    size_t days;
    UmiDecimal minimumBalance;
    UmiFinancialDate minimumDate, firstNegativeDate;
    bool hasNegativeBalance;
} UmiBankStatementTimelineSummary;

/* Create an immutable, owned daily trace for one account/currency and inclusive
 * provider-calendar booking-date range. Validation and final totals reuse the
 * statement review owner. A row exists only for a date with selected booked or
 * pending observations. Dates without observations keep the preceding balance
 * and are not materialised. Rows are sorted by date, independent of input order.
 * Opening means immediately before firstDate; each row is end-of-day. Pending
 * entries are counted but never affect money. Opening participates in minimum
 * and first-negative observations; equal minima keep the earliest date.
 * Every intermediate is exact and checked. An intermediate daily overflow can
 * refuse a timeline even if an aggregate review fits. No intraday ordering,
 * forecast, bank posting, network access or FX conversion is performed.
 * The feed and request may be released after this call. Failure clears *out. */
UmiStatus UmiBankStatementTimelineCreate(const UmiBankFeed *feed,
    const UmiBankStatementRequest *request, UmiBankStatementTimeline **out);
void UmiBankStatementTimelineDestroy(UmiBankStatementTimeline *timeline);
/* Copy values only on success. At returns NOT_FOUND beyond summary.days. Keep
 * calls and destruction on one owning thread; no internal pointers are exposed. */
UmiStatus UmiBankStatementTimelineRead(const UmiBankStatementTimeline *timeline,
    UmiBankStatementTimelineSummary *out);
UmiStatus UmiBankStatementTimelineAt(const UmiBankStatementTimeline *timeline,
    size_t index, UmiBankStatementDay *out);
/* Export opening, daily and closing rows from this same immutable snapshot.
 * Scope, supplied balances and pagination caveats repeat on every row. Decimal
 * values are exact text (not binary floating point); spreadsheet formula-like
 * strings are escaped by the shared CSV writer. Failure clears *out. This
 * contains private account data and creates no file by itself. */
UmiStatus UmiBankStatementTimelineExportCsv(const UmiBankStatementTimeline *timeline,
    UmiCsvDocument **out);
#ifdef __cplusplus
}
#endif
#endif
