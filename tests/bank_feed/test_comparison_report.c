/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_comparison_report.c
 *
 * PURPOSE:
 *   Preserve page limitations, formula safety and bounded export ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "csv_fixture.h"
#include "umicom/bank_feed/comparison.h"
#include "umicom/bank_feed/csv_import.h"
#undef CHECK
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); exit(1); } } while (0)

int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiBankFeed *before = NULL, *after = NULL;
    char *input = NULL;
    if (!strcmp(mode, "capacity")) {
        const size_t capacity = 4U * 1024U * 1024U;
        input = calloc(capacity, 1U);
        CHECK(input != NULL);
        strcpy(input, BANK_CSV_HEADER);
        /* Import limits count Unicode characters as well as storage bytes.
         * Retain the ASCII draft for review; its character counts exceed the
         * input contract before the intended export-capacity boundary. */
#if 0
        char information[2001]; memset(information, 'i', sizeof information - 1U);
        information[sizeof information - 1U] = '\0';
        char reference[841]; memset(reference, 'r', sizeof reference - 1U);
        reference[sizeof reference - 1U] = '\0';
#endif
        char information[2001], reference[841];
        for (size_t i = 0U; i < 500U; ++i) memcpy(information + i * 4U, "\xF0\x9F\x92\xA1", 4U);
        for (size_t i = 0U; i < 210U; ++i) memcpy(reference + i * 4U, "\xF0\x9F\x92\xA1", 4U);
        information[2000] = '\0'; reference[840] = '\0';
        size_t used = strlen(input);
        for (size_t i = 0U; i < UMI_BANK_FEED_MAX_ROWS; ++i) {
            int written = snprintf(input + used, capacity - used,
                "a,row-%zu,%s,%s,2026-10-12,GBP,1,BOOK,Immutable\n", i, reference, information);
            CHECK(written > 0 && (size_t)written < capacity - used);
            used += (size_t)written;
        }
        CHECK(UmiBankFeedCsvCreate(input, strlen(input), &before, NULL) == UMI_STATUS_OK);
    } else {
        const char *rows = !strcmp(mode, "empty") ? "" :
            !strcmp(mode, "formula") ? "a,t,=1+1,@command,2026-10-12,GBP,1,BOOK,Immutable\n" :
            !strcmp(mode, "unidentified") ? "a,,ref,details,2026-10-12,GBP,1,BOOK,Immutable\n" : BANK_CSV_CREDIT;
        input = malloc(strlen(BANK_CSV_HEADER) + strlen(rows) + 1U);
        CHECK(input != NULL);
        strcpy(input, BANK_CSV_HEADER); strcat(input, rows);
        CHECK(UmiBankFeedCsvCreate(input, strlen(input), &before, NULL) == UMI_STATUS_OK);
    }
    if (!strcmp(mode, "one-sided")) {
        CHECK(UmiBankFeedCsvCreate(BANK_CSV_HEADER, strlen(BANK_CSV_HEADER), &after, NULL) == UMI_STATUS_OK);
    } else {
        CHECK(UmiBankFeedCsvCreate(input, strlen(input), &after, NULL) == UMI_STATUS_OK);
    }
    free(input);
    UmiBankFeedComparison *owner = NULL;
    CHECK(UmiBankFeedComparisonCreate(before, after, &owner) == UMI_STATUS_OK);
    UmiBankFeedDestroy(before); UmiBankFeedDestroy(after);
    UmiCsvDocument *csv = (UmiCsvDocument *)1;
    if (!strcmp(mode, "invalid-api")) {
        CHECK(UmiBankFeedComparisonExportCsv(NULL, &csv) == UMI_STATUS_INVALID_ARGUMENT && csv == NULL);
        CHECK(UmiBankFeedComparisonExportCsv(owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "capacity")) {
        CHECK(UmiBankFeedComparisonExportCsv(owner, &csv) == UMI_STATUS_CAPACITY_EXCEEDED && csv == NULL);
        UmiBankFeedComparisonSummary summary;
        CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
        CHECK(summary.same == UMI_BANK_FEED_MAX_ROWS);
    } else {
        CHECK(UmiBankFeedComparisonExportCsv(owner, &csv) == UMI_STATUS_OK && csv != NULL);
        size_t expected = !strcmp(mode, "empty") ? 2U : !strcmp(mode, "one-sided") ? 3U : 4U;
        CHECK(UmiCsvDocumentRows(csv) == expected);
        const char *text = UmiCsvDocumentData(csv);
        CHECK(strstr(text, "before_previous_page") != NULL && strstr(text, "after_next_page") != NULL);
        CHECK(strstr(text, "absence is not deletion") != NULL);
        if (!strcmp(mode, "formula")) {
            CHECK(strstr(text, "\"'=1+1\"") != NULL && strstr(text, "\"'@command\"") != NULL);
        } else if (!strcmp(mode, "one-sided")) CHECK(strstr(text, "\"before-only\"") != NULL);
        else if (!strcmp(mode, "unidentified")) {
            CHECK(strstr(text, "\"before-unidentified\"") != NULL);
            CHECK(strstr(text, "\"after-unidentified\"") != NULL);
        } else if (strcmp(mode, "empty") != 0) CHECK(strstr(text, "\"same\"") != NULL);
        UmiBankFeedComparisonDestroy(owner);
        owner = NULL;
        CHECK(strstr(UmiCsvDocumentData(csv), "absence is not deletion") != NULL);
        UmiCsvDocumentDestroy(csv);
    }
    UmiBankFeedComparisonDestroy(owner);
    return 0;
}
