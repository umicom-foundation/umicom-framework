/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_cash_admission.c
 *
 * PURPOSE:
 *   Verify complete cash admission and unchanged state on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "operational_fixture.h"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiOpenTmsCashBook book, before;
    UmiOpenTmsCashRecord row = CashRow();
    UmiOpenTmsCashMeasures result = {123, 456};
    umi_open_tms_cash_book_init(&book);
    if (strcmp(test, "empty") == 0) {
        double total = -1;
        REQUIRE(umi_open_tms_cash_book_headroom_checked(&book, &total) == UMI_STATUS_OK && total == 0);
        return 0;
    }
    REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(umi_open_tms_cash_record_measure(&row, &result) == UMI_STATUS_OK);
        REQUIRE(result.netForecast == 650 && result.headroom == 450);
        REQUIRE(umi_open_tms_cash_book_find(&book, "account", "GBP") == &book.records[0]);
        row.projected = -10;
        REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_OK);
        REQUIRE(book.count == 1 && book.revision == 3);
    } else if (strcmp(test, "row-text") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsCashRecord, accountId), offsetof(UmiOpenTmsCashRecord, currency)};
        const size_t lengths[] = {64, 8};
        for (size_t i = 0; i < 2; ++i) {
            row = CashRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &book, sizeof book);
            REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &book, sizeof book) == 0);
            ((char *)&row)[offsets[i]] = '\0';
            REQUIRE(umi_open_tms_cash_record_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(test, "row-numbers") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsCashRecord, settled), offsetof(UmiOpenTmsCashRecord, projected), offsetof(UmiOpenTmsCashRecord, minimum), offsetof(UmiOpenTmsCashRecord, inflows), offsetof(UmiOpenTmsCashRecord, outflows)};
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < 5; ++i) for (size_t j = 0; j < 3; ++j) {
            row = CashRow(); memcpy((char *)&row + offsets[i], &bad[j], sizeof(double));
            memcpy(&before, &book, sizeof book);
            REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "stored-row") == 0 || strcmp(test, "duplicate") == 0) {
        if (strcmp(test, "count") == 0) book.count = SIZE_MAX;
        if (strcmp(test, "stored-row") == 0) memset(book.records[0].currency, 'x', 8);
        if (strcmp(test, "duplicate") == 0) { book.records[1] = row; book.count = 2; }
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_cash_book_find(&book, "account", "GBP") == NULL);
        REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) != UMI_STATUS_OK);
        double value = 99;
        REQUIRE(umi_open_tms_cash_book_headroom_checked(&book, &value) != UMI_STATUS_OK && value == 99);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "revision") == 0) {
        book.revision = 0; REQUIRE(umi_open_tms_cash_book_validate(&book) == UMI_STATUS_INVALID_STATE);
        book.revision = UINT64_MAX; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        REQUIRE(umi_open_tms_cash_book_validate(&book) == UMI_STATUS_OK);
    } else if (strcmp(test, "stale") == 0) {
        row.eventMilliseconds = 0; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_OPEN_TMS_CASH_CAPACITY; ++i) {
            book.records[i] = row; snprintf(book.records[i].accountId, 64, "account.%zu", i);
        }
        book.count = UMI_OPEN_TMS_CASH_CAPACITY; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_cash_book_validate(&book) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        row = book.records[255]; row.projected = 50;
        REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_OK && book.count == 256);
    } else if (strcmp(test, "query") == 0) {
        char query[64]; memset(query, 'x', sizeof query);
        REQUIRE(umi_open_tms_cash_book_find(&book, query, "GBP") == NULL);
        REQUIRE(umi_open_tms_cash_book_find(&book, "", "GBP") == NULL);
        REQUIRE(umi_open_tms_cash_book_find(&book, "account", query) == NULL);
    } else if (strcmp(test, "currencies") == 0) {
        strcpy(row.currency, "USD"); REQUIRE(umi_open_tms_cash_book_upsert(&book, &row) == UMI_STATUS_OK);
        double value = 88;
        REQUIRE(umi_open_tms_cash_book_headroom_checked(&book, &value) == UMI_STATUS_INVALID_STATE && value == 88);
    } else if (strcmp(test, "overflow") == 0) {
        row.projected = DBL_MAX; row.inflows = DBL_MAX;
        REQUIRE(umi_open_tms_cash_record_measure(&row, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(result.netForecast == 123 && result.headroom == 456);
        REQUIRE(umi_open_tms_cash_record_headroom(&row) == 0);
        row.inflows = 0; row.outflows = 0; row.minimum = 0;
        book.records[0] = row; strcpy(row.accountId, "second"); book.records[1] = row; book.count = 2;
        double value = 77;
        REQUIRE(umi_open_tms_cash_book_headroom_checked(&book, &value) == UMI_STATUS_CAPACITY_EXCEEDED && value == 77);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_open_tms_cash_record_measure(NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(result.netForecast == 123 && result.headroom == 456);
        REQUIRE(umi_open_tms_cash_record_measure(&row, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_open_tms_cash_book_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
