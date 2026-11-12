/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_rate_admission.c
 *
 * PURPOSE:
 *   Verify rate boundaries and failure-preserving observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/financial_platform/service.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    UmiFinancialRateBook book, before;
    UmiFinancialRateRecord row = {"GBP", "USD", 1.25, 1000U};
    UmiFinancialRateRecord found, saved;
    memset(&found, 0xa5, sizeof found); memcpy(&saved, &found, sizeof found);
    double amount = 77;
    umi_financial_rate_book_init(&book);
    REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", 100, &amount) == UMI_STATUS_OK && amount == 125);
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", -100, &amount) == UMI_STATUS_OK && amount == -125);
        REQUIRE(umi_financial_rate_book_find(&book, "GBP", "USD", &found) == UMI_STATUS_OK && found.rate == 1.25);
    } else if (strcmp(test, "identity") == 0) {
        /* Some Windows signbit macros expose a float conversion in an
         * unselected branch under strict warnings. Use the double copysign
         * function and an explicit zero check to preserve the exact assertion.
         * Keep the previous expression for review and cover both zero signs. */
#if 0
        REQUIRE(umi_financial_rate_book_convert(&book, "EUR", "EUR", -0.0, &amount) == UMI_STATUS_OK && signbit(amount));
#endif
        REQUIRE(umi_financial_rate_book_convert(&book, "EUR", "EUR", -0.0, &amount) == UMI_STATUS_OK);
        REQUIRE(amount == 0.0 && copysign(1.0, amount) == -1.0);
        REQUIRE(umi_financial_rate_book_convert(&book, "EUR", "EUR", +0.0, &amount) == UMI_STATUS_OK);
        REQUIRE(amount == 0.0 && copysign(1.0, amount) == 1.0);
        book.count = SIZE_MAX; amount = 77;
        REQUIRE(umi_financial_rate_book_convert(&book, "EUR", "EUR", 5, &amount) == UMI_STATUS_CAPACITY_EXCEEDED && amount == 77);
    } else if (strcmp(test, "text") == 0) {
        memset(row.base, 'x', sizeof row.base); memcpy(&before, &book, sizeof book);
        REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        strcpy(row.base, "GBP"); memset(row.quote, 'x', sizeof row.quote);
        REQUIRE(umi_financial_rate_record_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        row.quote[0] = '\0'; REQUIRE(umi_financial_rate_record_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "rate") == 0) {
        const double bad[] = {NAN, INFINITY, -INFINITY, 0, -1};
        for (size_t i = 0; i < 5; ++i) {
            row.rate = bad[i]; memcpy(&before, &book, sizeof book);
            REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0 || strcmp(test, "stored-text") == 0) {
        if (strcmp(test, "count") == 0) book.count = SIZE_MAX;
        if (strcmp(test, "duplicate") == 0) { book.records[1] = row; book.count = 2; }
        if (strcmp(test, "stored-text") == 0) memset(book.records[0].quote, 'x', 8);
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_financial_rate_book_upsert(&book, &row) != UMI_STATUS_OK);
        REQUIRE(umi_financial_rate_book_find(&book, "GBP", "USD", &found) != UMI_STATUS_OK);
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", 100, &amount) != UMI_STATUS_OK && amount == 77);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0 && memcmp(&saved, &found, sizeof found) == 0);
    } else if (strcmp(test, "revision") == 0) {
        book.revision = 0; REQUIRE(umi_financial_rate_book_validate(&book) == UMI_STATUS_INVALID_STATE);
        book.revision = UINT64_MAX; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", 100, &amount) == UMI_STATUS_OK && amount == 125);
    } else if (strcmp(test, "stale") == 0) {
        row.eventMilliseconds = 999; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        row.eventMilliseconds = 1000; row.rate = 1.5;
        REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_OK && book.count == 1);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_FINANCIAL_RATE_CAPACITY; ++i) {
            book.records[i] = row; snprintf(book.records[i].quote, 8, "Q%03u", (unsigned)i);
        }
        book.count = UMI_FINANCIAL_RATE_CAPACITY; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_financial_rate_book_validate(&book) == UMI_STATUS_OK);
        REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        row = book.records[127]; row.rate = 2; REQUIRE(umi_financial_rate_book_upsert(&book, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "query") == 0) {
        char key[8]; memset(key, 'x', sizeof key);
        REQUIRE(umi_financial_rate_book_find(&book, key, "USD", &found) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&saved, &found, sizeof found) == 0);
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", key, 5, &amount) == UMI_STATUS_CAPACITY_EXCEEDED && amount == 77);
        REQUIRE(umi_financial_rate_book_convert(&book, "", "", 5, &amount) == UMI_STATUS_INVALID_ARGUMENT && amount == 77);
    } else if (strcmp(test, "missing") == 0) {
        REQUIRE(umi_financial_rate_book_find(&book, "USD", "GBP", &found) == UMI_STATUS_NOT_FOUND);
        REQUIRE(memcmp(&saved, &found, sizeof found) == 0);
        REQUIRE(umi_financial_rate_book_convert(&book, "USD", "GBP", 5, &amount) == UMI_STATUS_NOT_FOUND && amount == 77);
    } else if (strcmp(test, "amount") == 0) {
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < 3; ++i) {
            REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", bad[i], &amount) == UMI_STATUS_INVALID_ARGUMENT && amount == 77);
            REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "GBP", bad[i], &amount) == UMI_STATUS_INVALID_ARGUMENT && amount == 77);
        }
    } else if (strcmp(test, "overflow") == 0) {
        book.records[0].rate = DBL_MAX;
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", 2, &amount) == UMI_STATUS_CAPACITY_EXCEEDED && amount == 77);
    } else if (strcmp(test, "underflow") == 0) {
        book.records[0].rate = 0.5;
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", DBL_TRUE_MIN, &amount) == UMI_STATUS_CAPACITY_EXCEEDED && amount == 77);
        book.records[0].rate = 1;
        REQUIRE(umi_financial_rate_book_convert(&book, "GBP", "USD", DBL_TRUE_MIN, &amount) == UMI_STATUS_OK && amount == DBL_TRUE_MIN);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_rate_book_convert(NULL, "GBP", "USD", 1, &amount) == UMI_STATUS_INVALID_ARGUMENT && amount == 77);
        REQUIRE(umi_financial_rate_book_convert(&book, NULL, "USD", 1, &amount) == UMI_STATUS_INVALID_ARGUMENT && amount == 77);
        REQUIRE(umi_financial_rate_book_find(&book, "GBP", "USD", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
