/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_csv_import.c
 * PURPOSE: Reject ambiguous statement fields and verify exact shared reconciliation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "csv_fixture.h"
#include "umicom/bank_feed/csv_import.h"
#include "umicom/bank_feed/statement.h"
static int Bad(const char *find, const char *replace, UmiStatus expected, size_t column)
{
    char *text = BankReplace(BANK_CSV_HEADER BANK_CSV_CREDIT, find, replace);
    CHECK(text != NULL);
    UmiBankFeed *feed = (UmiBankFeed *)1;
    UmiBankFeedCsvIssue issue = {99U, 99U};
    CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == expected);
    CHECK(feed == NULL && issue.column == column);
    free(text);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiBankFeed *feed = NULL;
    UmiBankFeedCsvIssue issue;
    if (!strcmp(mode, "values") || !strcmp(mode, "reconcile") || !strcmp(mode, "ownership")) {
        char *text = malloc(sizeof(BANK_CSV_TEXT));
        CHECK(text != NULL);
        memcpy(text, BANK_CSV_TEXT, sizeof(BANK_CSV_TEXT));
        CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == UMI_STATUS_OK);
        memset(text, 'x', strlen(text)); free(text);
        UmiBankFeedRow row;
        CHECK(UmiBankFeedAt(feed, 1U, &row) == UMI_STATUS_OK);
        CHECK(!row.credit && row.amount.coefficient == 225 && row.amount.scale == 2U);
        CHECK(!strcmp(row.bookingTime, "2026-10-12"));
        UmiBankFeedTotals totals;
        CHECK(UmiBankFeedSum(feed, "practice-account", "GBP", &totals) == UMI_STATUS_OK);
        CHECK(totals.bookedNet.coefficient == 1025 && totals.bookedNet.scale == 2U);
        CHECK(totals.pendingNet.coefficient == -99900 && totals.pendingRows == 1U);
        if (!strcmp(mode, "reconcile")) {
            UmiBankStatementRequest request = {"practice-account", "GBP",
                {2026, 10, 12}, {2026, 10, 12}, {10000, 2}, {11025, 2}};
            UmiBankStatementResult result;
            CHECK(UmiBankStatementReview(feed, &request, &result) == UMI_STATUS_OK);
            CHECK(result.balancesMatch && result.bookedRows == 2U && result.pendingRows == 1U);
        }
        if (!strcmp(mode, "ownership")) {
            UmiBankFeed *copy = NULL;
            CHECK(UmiBankFeedClone(feed, &copy) == UMI_STATUS_OK);
            UmiBankFeedDestroy(feed); feed = copy;
            CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK);
            CHECK(!strcmp(row.information, "Example credit"));
        }
    } else if (!strcmp(mode, "duplicate")) {
        const char *text = BANK_CSV_HEADER BANK_CSV_CREDIT BANK_CSV_CREDIT;
        CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(feed == NULL && issue.record == 3U && issue.column == 2U);
    } else if (!strcmp(mode, "account-scope")) {
        const char *text = BANK_CSV_HEADER BANK_CSV_CREDIT
            "other-account,credit-one,,,2026-10-12,GBP,9.00,BOOK,Immutable\n";
        CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == UMI_STATUS_OK);
        UmiBankFeedTotals totals;
        CHECK(UmiBankFeedSum(feed, "practice-account", "GBP", &totals) == UMI_STATUS_OK);
        CHECK(totals.bookedCredits.coefficient == 1250);
    } else if (!strcmp(mode, "missing-id")) {
        const char *text = BANK_CSV_HEADER
            "a,,,,2026-10-12,GBP,1,BOOK,Immutable\n"
            "a,,,,2026-10-12,GBP,1,BOOK,Immutable\n";
        CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == UMI_STATUS_OK);
        UmiBankFeedSummary summary;
        CHECK(UmiBankFeedRead(feed, &summary) == UMI_STATUS_OK);
        CHECK(summary.rows == 2U && summary.missingIdentifiers == 2U);
    } else if (!strcmp(mode, "header")) {
        CHECK(Bad("account,transaction", "Account,transaction", UMI_STATUS_PARSE_ERROR, 1U) == 0);
    } else if (!strcmp(mode, "columns")) {
        CHECK(Bad("12.50,BOOK", "12.50,extra,BOOK", UMI_STATUS_PARSE_ERROR, 0U) == 0);
    } else if (!strcmp(mode, "date")) {
        CHECK(Bad("2026-10-12", "2026-02-30", UMI_STATUS_PARSE_ERROR, 5U) == 0);
        CHECK(Bad("2026-10-12", "12/10/2026", UMI_STATUS_PARSE_ERROR, 5U) == 0);
    } else if (!strcmp(mode, "currency")) {
        CHECK(Bad("GBP", "gbp", UMI_STATUS_PARSE_ERROR, 6U) == 0);
        CHECK(Bad("GBP", "GB", UMI_STATUS_PARSE_ERROR, 6U) == 0);
    } else if (!strcmp(mode, "amount")) {
        const char *bad[] = {"1e2", "+12.50", " 12.50", "--1", "", "99999999999999"};
        for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i)
            CHECK(Bad("12.50", bad[i], UMI_STATUS_PARSE_ERROR, 7U) == 0);
    } else if (!strcmp(mode, "precision")) {
        CHECK(Bad("12.50", "1.123456", UMI_STATUS_PARSE_ERROR, 7U) == 0);
    } else if (!strcmp(mode, "status")) {
        CHECK(Bad("BOOK", "Unknown", UMI_STATUS_PARSE_ERROR, 8U) == 0);
    } else if (!strcmp(mode, "mutability")) {
        CHECK(Bad("Immutable", "Unknown", UMI_STATUS_PARSE_ERROR, 9U) == 0);
        const char *text = BANK_CSV_HEADER
            "a,id,,,2026-10-12,GBP,1,BOOK,Mutable\n";
        CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == UMI_STATUS_OK);
        UmiBankFeedRow row;
        CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK && row.mutableRecord);
    } else if (!strcmp(mode, "multiline") || !strcmp(mode, "formula")) {
        const char *value = !strcmp(mode, "multiline") ? "\"line one,\nline \"\"two\"\"\"" : "=SUM(A1:A2)";
        char *text = BankReplace(BANK_CSV_HEADER BANK_CSV_CREDIT, "Example credit", value);
        CHECK(text != NULL);
        CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == UMI_STATUS_OK);
        free(text);
        UmiCsvDocument *csv = NULL;
        CHECK(UmiBankFeedExportCsv(feed, &csv) == UMI_STATUS_OK);
        CHECK(strstr(UmiCsvDocumentData(csv), !strcmp(mode, "multiline") ?
                     "line one,\nline \"\"two\"\"" : "'=SUM(A1:A2)") != NULL);
        UmiCsvDocumentDestroy(csv);
    } else if (!strcmp(mode, "late-failure")) {
        const char *text = BANK_CSV_HEADER BANK_CSV_CREDIT
            "a,id,,,2026-10-12,GBP,1.234567,BOOK,Immutable\n";
        CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, &issue) == UMI_STATUS_PARSE_ERROR);
        CHECK(feed == NULL && issue.record == 3U);
    } else if (!strcmp(mode, "empty")) {
        CHECK(UmiBankFeedCsvCreate(BANK_CSV_HEADER, strlen(BANK_CSV_HEADER), &feed, &issue) == UMI_STATUS_OK);
        UmiBankFeedSummary summary;
        CHECK(UmiBankFeedRead(feed, &summary) == UMI_STATUS_OK && summary.rows == 0U);
    } else if (!strcmp(mode, "row-limit")) {
        const char *row = "a,,,,2026-10-12,GBP,1,BOOK,Immutable\n";
        size_t length = strlen(BANK_CSV_HEADER) + (UMI_BANK_FEED_MAX_ROWS + 1U) * strlen(row);
        char *text = malloc(length + 1U);
        CHECK(text != NULL);
        strcpy(text, BANK_CSV_HEADER);
        size_t at = strlen(text);
        for (size_t i = 0U; i <= UMI_BANK_FEED_MAX_ROWS; ++i) {
            memcpy(text + at, row, strlen(row)); at += strlen(row);
        }
        text[at] = '\0';
        CHECK(UmiBankFeedCsvCreate(text, at, &feed, &issue) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(feed == NULL && issue.record == UMI_BANK_FEED_MAX_ROWS + 2U);
        /* Exactly the documented limit must remain usable. */
        at -= strlen(row); text[at] = '\0';
        CHECK(UmiBankFeedCsvCreate(text, at, &feed, &issue) == UMI_STATUS_OK);
        free(text);
    } else if (!strcmp(mode, "invalid")) {
        CHECK(UmiBankFeedCsvCreate(NULL, 1U, &feed, &issue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(feed == NULL && issue.record == 0U && issue.column == 0U);
        CHECK(UmiBankFeedCsvCreate("x", 1U, NULL, &issue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankFeedCsvCreate("x", (size_t)UMI_BANK_FEED_MAX_BYTES + 1U, &feed, &issue) ==
              UMI_STATUS_CAPACITY_EXCEEDED && feed == NULL);
    } else if (!strcmp(mode, "field-limit")) {
        char account[42];
        memset(account, 'a', sizeof account - 1U); account[sizeof account - 1U] = '\0';
        CHECK(Bad("practice-account", account, UMI_STATUS_CAPACITY_EXCEEDED, 1U) == 0);
    } else if (!strcmp(mode, "utf8")) {
        CHECK(Bad("Example credit", "\xc0\xaf", UMI_STATUS_PARSE_ERROR, 0U) == 0);
    } else CHECK(0);
    UmiBankFeedDestroy(feed);
    return 0;
}
