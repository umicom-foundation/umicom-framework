/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_timeline.c
 * PURPOSE: Check chronological statement balances, source isolation and exact refusal paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/bank_feed/statement_timeline.h"
#include <stdint.h>
#undef CHECK
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); exit(1); } } while (0)
#define ROW(id,when,amount,direction,status,currency,account) \
 "{\"AccountId\":\"" account "\",\"TransactionId\":\"" id "\",\"BookingDateTime\":\"" when "\",\"Amount\":{\"Amount\":\"" amount "\",\"Currency\":\"" currency "\"},\"CreditDebitIndicator\":\"" direction "\",\"Status\":\"" status "\"}"
static const char rows[] =
 ROW("credit", "2026-10-12T23:00:00-05:00", "10.00", "Credit", "BOOK", "GBP", "practice-account") ","
 ROW("pending-late", "2026-10-13T00:00:00Z", "1000", "Debit", "PDNG", "GBP", "practice-account") ","
 ROW("debit", "2026-10-11T00:00:00+14:00", "15.00", "Debit", "BOOK", "GBP", "practice-account") ","
 ROW("fee", "2026-10-12T00:00:00Z", "2", "Debit", "BOOK", "GBP", "practice-account") ","
 ROW("pending", "2026-10-10T00:00:00Z", "2000", "Credit", "PDNG", "GBP", "practice-account") ","
 ROW("before", "2026-10-09T23:00:00Z", "99", "Credit", "BOOK", "GBP", "practice-account") ","
 ROW("after", "2026-10-14T00:00:00Z", "1", "Credit", "BOOK", "GBP", "practice-account") ","
 ROW("euro", "2026-10-11T00:00:00Z", "700", "Credit", "BOOK", "EUR", "practice-account") ","
 ROW("other", "2026-10-11T00:00:00Z", "600", "Credit", "BOOK", "GBP", "other-account");
static UmiBankFeed *Feed(const char *input) {
    char *json = BankFixture(input); CHECK(json != NULL);
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCreate(json, strlen(json), &feed) == UMI_STATUS_OK);
    free(json); return feed;
}
static int Exact(UmiDecimal value, int64_t coefficient, uint8_t scale) {
    int comparison = 2;
    CHECK(UmiDecimalCompare(value, (UmiDecimal){coefficient, scale}, &comparison) == UMI_STATUS_OK);
    return comparison == 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiBankFeed *feed = Feed(rows);
    char account[32] = "practice-account";
    UmiBankStatementRequest request = {account, "GBP", {2026, 10, 10}, {2026, 10, 13}, {1000, 2}, {300, 2}};
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(mode, "pending-only")) { request.lastDate = request.firstDate; request.closingBalance = request.openingBalance; }
    else if (!strcmp(mode, "mismatch")) request.closingBalance = (UmiDecimal){301, 2};
    else if (!strcmp(mode, "row-evidence")) {
        UmiBankFeedDestroy(feed);
        feed = Feed("{\"AccountId\":\"practice-account\",\"BookingDateTime\":\"2026-10-11T00:00:00Z\","
                    "\"Amount\":{\"Amount\":\"2\",\"Currency\":\"GBP\"},\"CreditDebitIndicator\":\"Credit\","
                    "\"Status\":\"BOOK\",\"TransactionMutability\":\"Mutable\"},"
                    "{\"AccountId\":\"practice-account\",\"BookingDateTime\":\"2026-10-11T00:00:00Z\","
                    "\"Amount\":{\"Amount\":\"900\",\"Currency\":\"GBP\"},\"CreditDebitIndicator\":\"Debit\","
                    "\"Status\":\"PDNG\"}");
        request.closingBalance = (UmiDecimal){12, 0};
    }
    else if (!strcmp(mode, "empty-period")) {
        request.firstDate.month = request.lastDate.month = 11; request.closingBalance = request.openingBalance;
    } else if (!strcmp(mode, "negative-opening")) {
        request.openingBalance = (UmiDecimal){-1, 0}; request.closingBalance = (UmiDecimal){-8, 0};
    } else if (!strcmp(mode, "currency-isolation")) {
        request.currency = "EUR"; request.closingBalance = (UmiDecimal){710, 0};
    } else if (!strcmp(mode, "invalid")) {
        request.firstDate.day = 14; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "overflow")) {
        request.openingBalance = (UmiDecimal){INT64_MIN, 0}; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(mode, "same-day") || !strcmp(mode, "earliest-tie") || !strcmp(mode, "intermediate-overflow")) {
        UmiBankFeedDestroy(feed);
        if (!strcmp(mode, "same-day")) {
            feed = Feed(ROW("out", "2026-10-11T01:00:00Z", "15", "Debit", "BOOK", "GBP", "practice-account") ","
                        ROW("in", "2026-10-11T23:00:00Z", "15", "Credit", "BOOK", "GBP", "practice-account"));
            request.openingBalance = request.closingBalance = (UmiDecimal){0, 0};
        } else if (!strcmp(mode, "earliest-tie")) {
            feed = Feed(ROW("out", "2026-10-11T00:00:00Z", "15", "Debit", "BOOK", "GBP", "practice-account") ","
                        ROW("in", "2026-10-12T00:00:00Z", "15", "Credit", "BOOK", "GBP", "practice-account") ","
                        ROW("again", "2026-10-13T00:00:00Z", "15", "Debit", "BOOK", "GBP", "practice-account"));
            request.closingBalance = (UmiDecimal){-5, 0};
        } else {
            feed = Feed(ROW("in", "2026-10-11T00:00:00Z", "1", "Credit", "BOOK", "GBP", "practice-account") ","
                        ROW("out", "2026-10-12T00:00:00Z", "1", "Debit", "BOOK", "GBP", "practice-account"));
            request.openingBalance = request.closingBalance = (UmiDecimal){INT64_MAX, 0};
            UmiBankStatementResult aggregate;
            CHECK(UmiBankStatementReview(feed, &request, &aggregate) == UMI_STATUS_OK);
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    } else if (!strcmp(mode, "precision")) {
        UmiBankFeedDestroy(feed);
        feed = Feed(ROW("tiny", "2026-10-11T00:00:00Z", "0.00001", "Credit", "BOOK", "GBP", "practice-account"));
        request.closingBalance = (UmiDecimal){1000001, 5};
    } else if (!strcmp(mode, "pagination") || !strcmp(mode, "csv-formula")) {
        char *json = BankFixture(rows); CHECK(json != NULL);
        char *changed;
        if (!strcmp(mode, "pagination"))
            changed = BankReplace(json, "{\"Data\":", "{\"Links\":{\"Next\":\"https://example.invalid/next\"},\"Data\":");
        else {
            changed = BankReplace(json, "practice-account", "=1+2");
            strcpy(account, "=1+2"); request.closingBalance = (UmiDecimal){20, 0};
        }
        CHECK(changed != NULL); UmiBankFeedDestroy(feed); feed = NULL;
        CHECK(UmiBankFeedCreate(changed, strlen(changed), &feed) == UMI_STATUS_OK);
        free(changed); free(json);
    } else if (!strcmp(mode, "capacity")) {
        char *many = calloc(400000U, 1U); CHECK(many != NULL); size_t used = 0U;
        for (size_t i = 0U; i < UMI_BANK_FEED_MAX_ROWS; ++i) {
            int written = snprintf(many + used, 400000U - used,
                "%s{\"AccountId\":\"practice-account\",\"TransactionId\":\"id-%zu\",\"BookingDateTime\":\"%04u-01-01T00:00:00Z\",\"Amount\":{\"Amount\":\"1\",\"Currency\":\"GBP\"},\"CreditDebitIndicator\":\"Credit\",\"Status\":\"BOOK\"}",
                i ? "," : "", i, 2000U + (unsigned)i);
            CHECK(written > 0 && (size_t)written < 400000U - used); used += (size_t)written;
        }
        UmiBankFeedDestroy(feed); feed = Feed(many); free(many);
        request.firstDate = (UmiFinancialDate){2000, 1, 1}; request.lastDate = (UmiFinancialDate){3023, 1, 1};
        request.openingBalance = (UmiDecimal){0, 0}; request.closingBalance = (UmiDecimal){1024, 0};
    } else CHECK(!strcmp(mode, "chronology") || !strcmp(mode, "ownership") || !strcmp(mode, "bounds"));
    UmiBankStatementTimeline *timeline = (UmiBankStatementTimeline *)(uintptr_t)1;
    CHECK(UmiBankStatementTimelineCreate(feed, &request, &timeline) == expected);
    if (expected != UMI_STATUS_OK) { CHECK(timeline == NULL); UmiBankFeedDestroy(feed); return 0; }
    UmiBankStatementTimelineSummary summary;
    CHECK(UmiBankStatementTimelineRead(timeline, &summary) == UMI_STATUS_OK);
    CHECK(summary.review.balancesMatch == (strcmp(mode, "mismatch") != 0));
    if (!strcmp(mode, "mismatch")) CHECK(Exact(summary.review.difference, 1, 2));
    if (!strcmp(mode, "row-evidence")) {
        UmiBankStatementDay evidence;
        CHECK(UmiBankStatementTimelineAt(timeline, 0U, &evidence) == UMI_STATUS_OK);
        CHECK(evidence.bookedRows == 1U && evidence.pendingRows == 1U);
        CHECK(evidence.unidentifiedRows == 2U && evidence.mutableBookedRows == 1U);
        CHECK(Exact(evidence.closing, 12, 0));
    }
    if (!strcmp(mode, "chronology") || !strcmp(mode, "ownership") || !strcmp(mode, "bounds") || !strcmp(mode, "pagination")) {
        CHECK(summary.days == 4U && summary.review.bookedRows == 3U && summary.review.pendingRows == 2U);
        CHECK(summary.review.outsidePeriodRows == 2U && Exact(summary.minimumBalance, -5, 0));
        CHECK(summary.hasNegativeBalance && summary.firstNegativeDate.day == 11U && summary.minimumDate.day == 11U);
        const int64_t balances[] = {10, -5, 3, 3};
        for (size_t i = 0U; i < 4U; ++i) {
            UmiBankStatementDay day; CHECK(UmiBankStatementTimelineAt(timeline, i, &day) == UMI_STATUS_OK);
            CHECK(day.date.day == 10U + i && Exact(day.closing, balances[i], 0));
        }
    } else if (!strcmp(mode, "pending-only")) {
        CHECK(summary.days == 1U && summary.review.bookedRows == 0U && !summary.hasNegativeBalance);
        CHECK(Exact(summary.minimumBalance, 10, 0));
    } else if (!strcmp(mode, "empty-period")) CHECK(summary.days == 0U && Exact(summary.minimumBalance, 10, 0));
    else if (!strcmp(mode, "negative-opening")) CHECK(summary.firstNegativeDate.day == 10U && Exact(summary.minimumBalance, -16, 0));
    else if (!strcmp(mode, "earliest-tie")) CHECK(summary.minimumDate.day == 11U && Exact(summary.minimumBalance, -5, 0));
    else if (!strcmp(mode, "same-day")) CHECK(summary.days == 1U && !summary.hasNegativeBalance && Exact(summary.minimumBalance, 0, 0));
    else if (!strcmp(mode, "precision")) CHECK(Exact(summary.review.expectedClosing, 1000001, 5));
    else if (!strcmp(mode, "currency-isolation")) CHECK(summary.days == 1U && summary.review.bookedRows == 1U);
    else if (!strcmp(mode, "capacity")) CHECK(summary.days == UMI_BANK_FEED_MAX_ROWS);
    UmiBankFeedDestroy(feed); feed = NULL;
    strcpy(account, "changed-after-create"); /* Export must retain the original copied identity. */
    UmiCsvDocument *csv = NULL;
    CHECK(UmiBankStatementTimelineExportCsv(timeline, &csv) == UMI_STATUS_OK);
    CHECK(UmiCsvDocumentRows(csv) == summary.days + 3U);
    const char *text = UmiCsvDocumentData(csv);
    CHECK(strstr(text, "completeness and authenticity unverified") && strstr(text, "pending excluded"));
    CHECK(strstr(text, "changed-after-create") == NULL);
    CHECK(strstr(text, "other-account") == NULL);
    if (!strcmp(mode, "csv-formula")) CHECK(strstr(text, "'=1+2"));
    if (!strcmp(mode, "pagination")) CHECK(summary.review.hasNextPage);
    UmiCsvDocumentDestroy(csv);
    UmiBankStatementDay day, saved;
    memset(&day, 0x5a, sizeof day); memcpy(&saved, &day, sizeof day);
    CHECK(UmiBankStatementTimelineAt(timeline, summary.days, &day) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&day, &saved, sizeof day) == 0);
    CHECK(UmiBankStatementTimelineRead(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBankStatementTimelineRead(timeline, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBankStatementTimelineAt(timeline, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBankStatementTimelineExportCsv(NULL, &csv) == UMI_STATUS_INVALID_ARGUMENT && csv == NULL);
    CHECK(UmiBankStatementTimelineExportCsv(timeline, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBankStatementTimelineCreate(NULL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBankStatementTimelineAt(NULL, 0U, &day) == UMI_STATUS_INVALID_ARGUMENT);
    UmiBankStatementTimelineDestroy(timeline);
    timeline = (UmiBankStatementTimeline *)(uintptr_t)1;
    CHECK(UmiBankStatementTimelineCreate(NULL, &request, &timeline) == UMI_STATUS_INVALID_ARGUMENT && timeline == NULL);
    UmiBankStatementTimelineDestroy(NULL);
    return 0;
}
