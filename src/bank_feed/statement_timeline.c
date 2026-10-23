/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement_timeline.c
 * PURPOSE: Calculate chronological daily balances using checked shared decimal operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "statement_private.h"
#include "statement_timeline_private.h"
#include <stdlib.h>
#include <string.h>

void UmiBankStatementTimelineDestroy(UmiBankStatementTimeline *timeline) { free(timeline); }
UmiStatus UmiBankStatementTimelineRead(const UmiBankStatementTimeline *timeline,
    UmiBankStatementTimelineSummary *out) {
    if (timeline == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = timeline->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiBankStatementTimelineAt(const UmiBankStatementTimeline *timeline,
    size_t index, UmiBankStatementDay *out) {
    if (timeline == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= timeline->summary.days) return UMI_STATUS_NOT_FOUND;
    *out = timeline->days[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiBankStatementTimelineCreate(const UmiBankFeed *feed,
    const UmiBankStatementRequest *request, UmiBankStatementTimeline **out) {
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiBankStatementResult review;
    UmiStatus status = UmiBankStatementReview(feed, request, &review);
    if (status != UMI_STATUS_OK) return status;
    if (strlen(request->account) >= sizeof ((UmiBankStatementTimeline *)0)->account)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Allocate off the stack. Neither source rows nor partly accumulated days
     * escape if a late date or intermediate amount cannot be represented. */
    UmiBankStatementTimeline *timeline = calloc(1U, sizeof *timeline);
    if (timeline == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    strcpy(timeline->account, request->account);
    strcpy(timeline->currency, request->currency);
    timeline->first = request->firstDate;
    timeline->last = request->lastDate;
    timeline->opening = request->openingBalance;
    timeline->reportedClosing = request->closingBalance;
    timeline->summary.review = review;
    timeline->summary.minimumBalance = timeline->opening;
    timeline->summary.minimumDate = timeline->first;
    if (timeline->opening.coefficient < 0) {
        timeline->summary.hasNegativeBalance = true;
        timeline->summary.firstNegativeDate = timeline->first;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < feed->summary.rows; ++i) {
        const UmiBankFeedRow *row = &feed->rows[i];
        if (!BankStatementAccount(row, request) || !BankStatementInPeriod(row, request)) continue;
        char dateText[11];
        memcpy(dateText, row->bookingTime, 10U); dateText[10] = '\0';
        UmiFinancialDate date;
        status = UmiBankStatementDate(dateText, &date);
        if (status != UMI_STATUS_OK) break;
        size_t at = 0U;
        while (at < timeline->summary.days && umi_financial_date_compare(timeline->days[at].date, date) < 0) ++at;
        if (at == timeline->summary.days || umi_financial_date_compare(timeline->days[at].date, date) != 0) {
            if (timeline->summary.days == UMI_BANK_FEED_MAX_ROWS) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
            memmove(&timeline->days[at + 1U], &timeline->days[at],
                (timeline->summary.days - at) * sizeof timeline->days[0]);
            timeline->days[at] = (UmiBankStatementDay){.date = date};
            ++timeline->summary.days;
        }
        UmiBankStatementDay *day = &timeline->days[at];
        if (row->transaction[0] == '\0') ++day->unidentifiedRows;
        if (!row->booked) { ++day->pendingRows; continue; }
        ++day->bookedRows;
        if (row->mutableRecord) ++day->mutableBookedRows;
        UmiDecimal *total = row->credit ? &day->credits : &day->debits;
        status = UmiDecimalAddExact(*total, row->amount, total);
    }
    UmiDecimal balance = timeline->opening;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < timeline->summary.days; ++i) {
        UmiBankStatementDay *day = &timeline->days[i];
        status = UmiDecimalSubtractExact(day->credits, day->debits, &day->netMovement);
        if (status == UMI_STATUS_OK) status = UmiDecimalAddExact(balance, day->netMovement, &day->closing);
        int comparison = 0;
        if (status == UMI_STATUS_OK) status = UmiDecimalCompare(day->closing, timeline->summary.minimumBalance, &comparison);
        if (status != UMI_STATUS_OK) break;
        balance = day->closing;
        if (comparison < 0) {
            timeline->summary.minimumBalance = balance;
            timeline->summary.minimumDate = day->date;
        }
        if (balance.coefficient < 0 && !timeline->summary.hasNegativeBalance) {
            timeline->summary.hasNegativeBalance = true;
            timeline->summary.firstNegativeDate = day->date;
        }
    }
    /* The final chronological balance must agree with the canonical aggregate
     * owner. Compare exact values across scales without forcing a rescale. */
    int comparison = 0;
    if (status == UMI_STATUS_OK) status = UmiDecimalCompare(balance, review.expectedClosing, &comparison);
    if (status == UMI_STATUS_OK && comparison != 0) status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK) *out = timeline;
    else UmiBankStatementTimelineDestroy(timeline);
    return status;
}
