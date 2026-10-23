/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement_timeline_report.c
 * PURPOSE: Keep exported daily balances attached to their account, assumptions and import limitations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "statement_timeline_private.h"
#include <stdio.h>

static void Date(UmiFinancialDate date, char out[16]) {
    (void)snprintf(out, 16U, "%04d-%02u-%02u", (int)date.year, (unsigned)date.month, (unsigned)date.day);
}
UmiStatus UmiBankStatementTimelineExportCsv(const UmiBankStatementTimeline *timeline,
    UmiCsvDocument **out) {
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (timeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiBankStatementTimelineSummary *summary = &timeline->summary;
    const UmiBankStatementResult *review = &summary->review;
    const char *headings[] = {"record_kind", "account_id", "currency", "first_booking_date",
        "last_booking_date", "row_date", "opening_balance", "reported_closing", "booked_credits",
        "booked_debits", "net_movement", "calculated_balance", "expected_closing", "difference",
        "arithmetic_matches", "booked_rows", "pending_rows", "mutable_booked_rows", "unidentified_rows",
        "outside_period_rows", "previous_page", "next_page", "minimum_balance", "minimum_date",
        "first_negative_date", "scope"};
    UmiCsvDocument *csv = NULL;
    UmiStatus status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &csv);
    UmiCsvCell cells[26];
    for (size_t i = 0U; i < 26U; ++i) cells[i] = UmiCsvText(headings[i]);
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(csv, cells, 26U);
    char first[16], last[16], minimum[16], negative[16] = "";
    Date(timeline->first, first); Date(timeline->last, last); Date(summary->minimumDate, minimum);
    if (summary->hasNegativeBalance) Date(summary->firstNegativeDate, negative);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary->days + 2U; ++i) {
        const bool opening = i == 0U, closing = i == summary->days + 1U;
        UmiBankStatementDay day = {.date = timeline->first, .closing = timeline->opening};
        if (closing) {
            day.date = timeline->last; day.closing = review->expectedClosing;
            day.credits = review->credits; day.debits = review->debits; day.netMovement = review->netMovement;
            day.bookedRows = review->bookedRows; day.pendingRows = review->pendingRows;
            day.mutableBookedRows = review->mutableBookedRows; day.unidentifiedRows = review->unidentifiedRows;
        } else if (!opening) day = timeline->days[i - 1U];
        char rowDate[16], numbers[9][UMI_DECIMAL_TEXT_MAX];
        Date(day.date, rowDate);
        const UmiDecimal values[] = {timeline->opening, timeline->reportedClosing, day.credits, day.debits,
            day.netMovement, day.closing, review->expectedClosing, review->difference, summary->minimumBalance};
        for (size_t j = 0U; status == UMI_STATUS_OK && j < 9U; ++j)
            status = UmiDecimalFormat(values[j], numbers[j], sizeof numbers[j]);
        if (status != UMI_STATUS_OK) break;
        cells[0] = UmiCsvText(opening ? "opening" : closing ? "closing" : "day");
        cells[1] = UmiCsvText(timeline->account); cells[2] = UmiCsvText(timeline->currency);
        cells[3] = UmiCsvText(first); cells[4] = UmiCsvText(last); cells[5] = UmiCsvText(rowDate);
        for (size_t j = 0U; j < 8U; ++j) cells[6U + j] = UmiCsvText(numbers[j]);
        cells[14] = UmiCsvText(review->balancesMatch ? "true" : "false");
        cells[15] = UmiCsvUnsigned(day.bookedRows); cells[16] = UmiCsvUnsigned(day.pendingRows);
        cells[17] = UmiCsvUnsigned(day.mutableBookedRows); cells[18] = UmiCsvUnsigned(day.unidentifiedRows);
        cells[19] = UmiCsvUnsigned(review->outsidePeriodRows);
        cells[20] = UmiCsvText(review->hasPreviousPage ? "true" : "false");
        cells[21] = UmiCsvText(review->hasNextPage ? "true" : "false");
        cells[22] = UmiCsvText(numbers[8]); cells[23] = UmiCsvText(minimum); cells[24] = UmiCsvText(negative);
        cells[25] = UmiCsvText("Imported snapshot; provider calendar dates; supplied opening; pending excluded; "
            "end-of-day only; dates without observations carry the prior balance; completeness and authenticity unverified");
        status = UmiCsvDocumentAppendRow(csv, cells, 26U);
    }
    if (status == UMI_STATUS_OK) *out = csv;
    else UmiCsvDocumentDestroy(csv);
    return status;
}
