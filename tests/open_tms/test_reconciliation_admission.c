/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_reconciliation_admission.c
 *
 * PURPOSE:
 *   Verify complete reconciliation admission and unchanged state on refusal.
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
    static UmiOpenTmsReconciliationBook book, before;
    UmiOpenTmsReconciliationItem row = ReconciliationRow();
    umi_open_tms_reconciliation_book_init(&book);
    REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        double coverage = -1;
        REQUIRE(book.breakCount == 1 && book.matchedCount == 0);
        REQUIRE(umi_open_tms_reconciliation_book_coverage_checked(&book, &coverage) == UMI_STATUS_OK && coverage == 0);
        REQUIRE(umi_open_tms_reconciliation_book_resolve(&book, "match") == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_reconciliation_book_coverage_checked(&book, &coverage) == UMI_STATUS_OK && coverage == 100);
        REQUIRE(book.breakCount == 0 && book.matchedCount == 0);
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_reconciliation_book_resolve(&book, "match") == UMI_STATUS_ALREADY_EXISTS);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        strcpy(row.id, "exact"); row.targetValue = 100.5;
        REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_OK && book.matchedCount == 1);
    } else if (strcmp(test, "text") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsReconciliationItem, id), offsetof(UmiOpenTmsReconciliationItem, sourceId), offsetof(UmiOpenTmsReconciliationItem, targetId)};
        for (size_t i = 0; i < 3; ++i) {
            row = ReconciliationRow(); memset((char *)&row + offsets[i], 'x', 64);
            memcpy(&before, &book, sizeof book);
            REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        }
    } else if (strcmp(test, "numbers") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsReconciliationItem, sourceValue), offsetof(UmiOpenTmsReconciliationItem, targetValue), offsetof(UmiOpenTmsReconciliationItem, tolerance)};
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < 3; ++i) for (size_t j = 0; j < 3; ++j) {
            row = ReconciliationRow(); memcpy((char *)&row + offsets[i], &bad[j], sizeof(double));
            REQUIRE(umi_open_tms_reconciliation_item_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "counters") == 0 || strcmp(test, "flags") == 0) {
        if (strcmp(test, "count") == 0) book.count = SIZE_MAX;
        if (strcmp(test, "counters") == 0) book.breakCount = 0;
        if (strcmp(test, "flags") == 0) book.items[0].matched = 1;
        memcpy(&before, &book, sizeof book); double coverage = 77;
        REQUIRE(umi_open_tms_reconciliation_book_resolve(&book, "match") != UMI_STATUS_OK);
        REQUIRE(umi_open_tms_reconciliation_book_coverage_checked(&book, &coverage) != UMI_STATUS_OK && coverage == 77);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "duplicate") == 0) {
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_ALREADY_EXISTS);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        book.items[1] = book.items[0]; book.count = 2; book.breakCount = 2;
        REQUIRE(umi_open_tms_reconciliation_book_validate(&book) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(test, "resolved-import") == 0) {
        umi_open_tms_reconciliation_book_init(&book); row.resolved = 1;
        REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_OK);
        REQUIRE(book.breakCount == 0 && book.items[0].resolved == 1);
        REQUIRE(umi_open_tms_reconciliation_book_validate(&book) == UMI_STATUS_OK);
        row.resolved = 2; REQUIRE(umi_open_tms_reconciliation_item_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "revision") == 0) {
        book.revision = 0; REQUIRE(umi_open_tms_reconciliation_book_validate(&book) == UMI_STATUS_INVALID_STATE);
        book.revision = UINT64_MAX; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_open_tms_reconciliation_book_resolve(&book, "match") == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_OPEN_TMS_RECONCILIATION_CAPACITY; ++i) {
            book.items[i] = row; snprintf(book.items[i].id, 64, "match.%zu", i);
        }
        book.count = UMI_OPEN_TMS_RECONCILIATION_CAPACITY; book.breakCount = book.count;
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_reconciliation_book_validate(&book) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "query") == 0) {
        char query[64]; memset(query, 'x', sizeof query); memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_reconciliation_book_resolve(&book, query) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "overflow") == 0) {
        strcpy(row.id, "overflow"); row.sourceValue = DBL_MAX; row.targetValue = -DBL_MAX;
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_reconciliation_book_add(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "null") == 0) {
        double coverage = 77;
        REQUIRE(umi_open_tms_reconciliation_book_coverage_checked(NULL, &coverage) == UMI_STATUS_INVALID_ARGUMENT && coverage == 77);
        REQUIRE(umi_open_tms_reconciliation_book_coverage_checked(&book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
