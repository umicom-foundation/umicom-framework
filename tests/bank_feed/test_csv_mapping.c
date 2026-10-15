/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_csv_mapping.c
 * PURPOSE: Check column interpretation, exact amounts and all-or-nothing statement imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "csv_fixture.h"
#include "umicom/bank_feed/csv_mapping.h"
#include "umicom/bank_feed/statement.h"
#define MAPPED_HEADER "ID,Ignored,Date,Amount,Description\n"
#define MAPPED_ROW "one,x,12/10/2026,12.50,\"shop, caf\xc3\xa9\"\n"
#define MAPPED_TEXT MAPPED_HEADER MAPPED_ROW
#define SPLIT_HEADER "ID,Ignored,Date,Debit,Credit,Description\n"
static UmiBankFeedCsvMapping Settings(void)
{
    UmiBankFeedCsvMapping mapping = {0};
    strcpy(mapping.headers[UMI_BANK_CSV_TRANSACTION], "ID");
    strcpy(mapping.headers[UMI_BANK_CSV_DATE], "Date");
    strcpy(mapping.headers[UMI_BANK_CSV_AMOUNT], "Amount");
    strcpy(mapping.headers[UMI_BANK_CSV_INFORMATION], "Description");
    strcpy(mapping.account, "practice-account");
    strcpy(mapping.currency, "GBP");
    mapping.dateOrder = UMI_BANK_CSV_DATE_DAY_FIRST;
    mapping.booked = true;
    mapping.mutableRecord = true;
    return mapping;
}
static int Reject(const char *text, const UmiBankFeedCsvMapping *mapping,
                   UmiStatus expected, size_t record, size_t column)
{
    UmiBankFeed *feed = (UmiBankFeed *)1;
    UmiBankFeedCsvIssue issue = {99U, 99U};
    CHECK(UmiBankFeedCsvMappedCreate(text, strlen(text), mapping, &feed, &issue) == expected);
    CHECK(feed == NULL && issue.record == record && issue.column == column);
    return 0;
}
/* Changing a single source cell while holding the mapping fixed makes error
 * locations observable against the original column order, not the internal schema. */
static int Changed(const char *find, const char *replacement, UmiStatus expected,
                    size_t record, size_t column)
{
    UmiBankFeedCsvMapping mapping = Settings();
    char *text = BankReplace(MAPPED_TEXT, find, replacement);
    CHECK(text != NULL);
    int result = Reject(text, &mapping, expected, record, column);
    free(text);
    return result;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiBankFeedCsvMapping mapping = Settings();
    UmiBankFeed *feed = NULL;
    UmiBankFeedCsvIssue issue;
    if (!strcmp(mode, "default-schema")) {
        UmiBankFeedCsvMappingInit(&mapping);
        CHECK(!mapping.booked && mapping.mutableRecord);
        CHECK(UmiBankFeedCsvMappedCreate(BANK_CSV_TEXT, strlen(BANK_CSV_TEXT),
                                           &mapping, &feed, &issue) == UMI_STATUS_OK);
        UmiBankStatementRequest request = {"practice-account", "GBP",
            {2026, 10, 12}, {2026, 10, 12}, {10000, 2}, {11025, 2}};
        UmiBankStatementResult result;
        CHECK(UmiBankStatementReview(feed, &request, &result) == UMI_STATUS_OK);
        CHECK(result.balancesMatch && result.bookedRows == 2U && result.pendingRows == 1U);
    } else if (!strcmp(mode, "reordered") || !strcmp(mode, "ownership") ||
               !strcmp(mode, "pending") || !strcmp(mode, "month-first")) {
        if (!strcmp(mode, "pending")) { mapping.booked = false; mapping.mutableRecord = false; }
        if (!strcmp(mode, "month-first")) mapping.dateOrder = UMI_BANK_CSV_DATE_MONTH_FIRST;
        char *text = malloc(sizeof MAPPED_TEXT);
        CHECK(text != NULL);
        memcpy(text, MAPPED_TEXT, sizeof MAPPED_TEXT);
        CHECK(UmiBankFeedCsvMappedCreate(text, strlen(text), &mapping, &feed, &issue) == UMI_STATUS_OK);
        memset(text, 'x', strlen(text)); free(text);
        memset(&mapping, 0, sizeof mapping);
        UmiBankFeedRow row;
        CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK);
        CHECK(!strcmp(row.account, "practice-account") && !strcmp(row.currency, "GBP"));
        CHECK(!strcmp(row.information, "shop, caf\xc3\xa9") && !strcmp(row.transaction, "one"));
        CHECK(!strcmp(row.bookingTime, !strcmp(mode, "month-first") ? "2026-12-10" : "2026-10-12"));
        CHECK(row.amount.coefficient == 1250 && row.amount.scale == 2U && row.credit);
        CHECK(row.booked == (strcmp(mode, "pending") != 0) && row.mutableRecord);
        CHECK(issue.record == 0U && issue.column == 0U);
    } else if (!strcmp(mode, "iso")) {
        mapping.dateOrder = UMI_BANK_CSV_DATE_ISO;
        const char *text = MAPPED_HEADER "one,x,2026-02-28,-12.50,example\n";
        CHECK(UmiBankFeedCsvMappedCreate(text, strlen(text), &mapping, &feed, NULL) == UMI_STATUS_OK);
        UmiBankFeedRow row;
        CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK && !row.credit);
    } else if (!strcmp(mode, "mapped-identity")) {
        mapping.account[0] = '\0'; mapping.currency[0] = '\0';
        strcpy(mapping.headers[UMI_BANK_CSV_ACCOUNT], "Account");
        strcpy(mapping.headers[UMI_BANK_CSV_CURRENCY], "Currency");
        const char *text = "ID,Ignored,Date,Amount,Description,Account,Currency\n"
            "same,x,12/10/2026,1,first,account-a,GBP\nsame,x,12/10/2026,2,second,account-b,USD\n";
        CHECK(UmiBankFeedCsvMappedCreate(text, strlen(text), &mapping, &feed, NULL) == UMI_STATUS_OK);
        UmiBankFeedTotals totals;
        CHECK(UmiBankFeedSum(feed, "account-b", "USD", &totals) == UMI_STATUS_OK);
        CHECK(totals.bookedNet.coefficient == 2 && totals.bookedRows == 1U);
    } else if (!strcmp(mode, "mapped-status")) {
        strcpy(mapping.headers[UMI_BANK_CSV_STATUS], "State");
        strcpy(mapping.headers[UMI_BANK_CSV_MUTABILITY], "Changeable");
        const char *text = "ID,Ignored,Date,Amount,Description,State,Changeable\n"
            "one,x,12/10/2026,1,first,PDNG,Immutable\n"
            "two,x,12/10/2026,2,second,BOOK,Immutable\n";
        CHECK(UmiBankFeedCsvMappedCreate(text, strlen(text), &mapping, &feed, NULL) == UMI_STATUS_OK);
        UmiBankFeedRow row;
        CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK && !row.booked && row.mutableRecord);
        CHECK(UmiBankFeedAt(feed, 1U, &row) == UMI_STATUS_OK && row.booked && !row.mutableRecord);
    } else if (strncmp(mode, "split", 5U) == 0) {
        mapping.amountMode = UMI_BANK_CSV_AMOUNT_SEPARATE;
        mapping.headers[UMI_BANK_CSV_AMOUNT][0] = '\0';
        strcpy(mapping.headers[UMI_BANK_CSV_DEBIT], "Debit");
        strcpy(mapping.headers[UMI_BANK_CSV_CREDIT], "Credit");
        if (!strcmp(mode, "split-both"))
            return Reject(SPLIT_HEADER "one,x,12/10/2026,1,2,example\n", &mapping, UMI_STATUS_PARSE_ERROR, 2U, 0U);
        if (!strcmp(mode, "split-empty"))
            return Reject(SPLIT_HEADER "one,x,12/10/2026,,,example\n", &mapping, UMI_STATUS_PARSE_ERROR, 2U, 0U);
        if (!strcmp(mode, "split-negative"))
            return Reject(SPLIT_HEADER "one,x,12/10/2026,-1,,example\n", &mapping, UMI_STATUS_PARSE_ERROR, 2U, 4U);
        if (!strcmp(mode, "split-precision"))
            return Reject(SPLIT_HEADER "one,x,12/10/2026,,1.123456,example\n", &mapping, UMI_STATUS_PARSE_ERROR, 2U, 5U);
        CHECK(!strcmp(mode, "split") || !strcmp(mode, "split-zero"));
        const char *text = !strcmp(mode, "split-zero")
            ? SPLIT_HEADER "one,x,12/10/2026,0.00,0,example\n"
            : SPLIT_HEADER "one,x,12/10/2026,,12.50,credit\n"
                           "two,x,12/10/2026,2.25,0.00,debit\n";
        CHECK(UmiBankFeedCsvMappedCreate(text, strlen(text), &mapping, &feed, NULL) == UMI_STATUS_OK);
        UmiBankFeedTotals totals;
        CHECK(UmiBankFeedSum(feed, "practice-account", "GBP", &totals) == UMI_STATUS_OK);
        if (!strcmp(mode, "split-zero")) CHECK(totals.bookedNet.coefficient == 0);
        else CHECK(totals.bookedNet.coefficient == 1025 && totals.bookedNet.scale == 2U);
    } else if (!strcmp(mode, "duplicate-id"))
        return Reject(MAPPED_TEXT MAPPED_ROW, &mapping, UMI_STATUS_ALREADY_EXISTS, 3U, 1U);
    else if (!strcmp(mode, "late-failure"))
        return Reject(MAPPED_TEXT "two,x,31/02/2026,1,example\n", &mapping, UMI_STATUS_PARSE_ERROR, 3U, 3U);
    else if (!strcmp(mode, "duplicate-header"))
        return Changed("Ignored", "ID", UMI_STATUS_ALREADY_EXISTS, 1U, 2U);
    else if (!strcmp(mode, "missing-header"))
        return Changed("Date", "date", UMI_STATUS_NOT_FOUND, 1U, 0U);
    else if (!strcmp(mode, "record-width"))
        return Changed("one,x,", "one,", UMI_STATUS_PARSE_ERROR, 2U, 0U);
    else if (!strcmp(mode, "date"))
        return Changed("12/10/2026", "1/10/2026", UMI_STATUS_PARSE_ERROR, 2U, 3U);
    else if (!strcmp(mode, "amount"))
        return Changed("12.50", " 12.50", UMI_STATUS_PARSE_ERROR, 2U, 4U);
    else if (!strcmp(mode, "header-text"))
        return Changed("Ignored", "\"two\nlines\"", UMI_STATUS_PARSE_ERROR, 1U, 2U);
    else if (!strcmp(mode, "formula")) {
        const char *text = MAPPED_HEADER "one,x,12/10/2026,1,=1+1\n";
        CHECK(UmiBankFeedCsvMappedCreate(text, strlen(text), &mapping, &feed, NULL) == UMI_STATUS_OK);
        UmiBankFeedRow row;
        CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK && !strcmp(row.information, "=1+1"));
        UmiCsvDocument *document = NULL;
        CHECK(UmiBankFeedExportCsv(feed, &document) == UMI_STATUS_OK);
        CHECK(strstr(UmiCsvDocumentData(document), "'=1+1") != NULL);
        UmiCsvDocumentDestroy(document);
    } else if (!strcmp(mode, "empty")) {
        CHECK(UmiBankFeedCsvMappedCreate(MAPPED_HEADER, strlen(MAPPED_HEADER), &mapping,
                                           &feed, NULL) == UMI_STATUS_OK);
        UmiBankFeedSummary summary;
        CHECK(UmiBankFeedRead(feed, &summary) == UMI_STATUS_OK && summary.rows == 0U);
    } else if (!strcmp(mode, "row-limit")) {
        size_t capacity = (UMI_BANK_FEED_MAX_ROWS + 1U) * 80U + sizeof MAPPED_HEADER;
        char *text = malloc(capacity);
        CHECK(text != NULL);
        size_t used = (size_t)snprintf(text, capacity, "%s", MAPPED_HEADER);
        for (size_t i = 0U; i <= UMI_BANK_FEED_MAX_ROWS; ++i)
            used += (size_t)snprintf(text + used, capacity - used, "%zu,x,12/10/2026,1,example\n", i);
        int result = Reject(text, &mapping, UMI_STATUS_CAPACITY_EXCEEDED,
                              UMI_BANK_FEED_MAX_ROWS + 2U, 0U);
        free(text); return result;
    } else if (!strcmp(mode, "invalid")) {
        feed = (UmiBankFeed *)1;
        CHECK(UmiBankFeedCsvMappedCreate(NULL, 0U, &mapping, &feed, &issue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(feed == NULL && issue.record == 0U && issue.column == 0U);
        CHECK(UmiBankFeedCsvMappedCreate(MAPPED_TEXT, strlen(MAPPED_TEXT), &mapping,
                                           NULL, &issue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankFeedCsvMappedCreate(MAPPED_TEXT, strlen(MAPPED_TEXT), NULL,
                                           &feed, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankFeedCsvMappedCreate(MAPPED_TEXT, UMI_BANK_FEED_MAX_BYTES + 1U,
                                           &mapping, &feed, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
        UmiBankFeedCsvMappingInit(NULL);
    } else {
        if (!strcmp(mode, "duplicate-role")) strcpy(mapping.headers[UMI_BANK_CSV_REFERENCE], "Date");
        else if (!strcmp(mode, "missing-account")) mapping.account[0] = '\0';
        else if (!strcmp(mode, "conflicting-account")) strcpy(mapping.headers[UMI_BANK_CSV_ACCOUNT], "ID");
        else if (!strcmp(mode, "currency")) strcpy(mapping.currency, "gbp");
        else if (!strcmp(mode, "unterminated")) memset(mapping.headers[0], 'x', sizeof mapping.headers[0]);
        else if (!strcmp(mode, "unknown-format")) mapping.dateOrder = (UmiBankCsvDateOrder)99;
        else if (!strcmp(mode, "invalid-utf8")) { mapping.account[0] = (char)0xff; mapping.account[1] = '\0'; }
        else if (!strcmp(mode, "account-limit")) memset(mapping.account, 'a', 41U);
        else { CHECK(!"Unknown case"); }
        return Reject(MAPPED_TEXT, &mapping, UMI_STATUS_INVALID_ARGUMENT, 0U, 0U);
    }
    UmiBankFeedDestroy(feed);
    return 0;
}
