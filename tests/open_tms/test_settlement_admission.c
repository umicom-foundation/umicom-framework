/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_settlement_admission.c
 *
 * PURPOSE:
 *   Verify complete settlement admission and unchanged state on refusal.
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
    static UmiOpenTmsSettlementBook book, before;
    UmiOpenTmsSettlementRecord row = SettlementRow();
    UmiOpenTmsSettlementMeasures result = {123, 456};
    umi_open_tms_settlement_book_init(&book);
    REQUIRE(umi_open_tms_settlement_book_add(&book, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(umi_open_tms_settlement_book_measure(&book, 1000, &result) == UMI_STATUS_OK && result.overdueCount == 0);
        REQUIRE(umi_open_tms_settlement_book_measure(&book, 1001, &result) == UMI_STATUS_OK && result.overdueCount == 1);
        REQUIRE(umi_open_tms_settlement_book_transition(&book, "payment", UMI_OPEN_TMS_SETTLEMENT_FAILED, 2) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_settlement_book_measure(&book, 1001, &result) == UMI_STATUS_OK && result.failedCount == 1);
        book.records[0].dueMilliseconds = 0;
        REQUIRE(umi_open_tms_settlement_book_measure(&book, UINT64_MAX, &result) == UMI_STATUS_OK && result.overdueCount == 0);
    } else if (strcmp(test, "text") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsSettlementRecord, id), offsetof(UmiOpenTmsSettlementRecord, sourceId), offsetof(UmiOpenTmsSettlementRecord, accountId), offsetof(UmiOpenTmsSettlementRecord, currency)};
        const size_t lengths[] = {64, 64, 64, 8};
        for (size_t i = 0; i < 4; ++i) {
            row = SettlementRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &book, sizeof book);
            REQUIRE(umi_open_tms_settlement_book_add(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        }
    } else if (strcmp(test, "numbers") == 0) {
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < 3; ++i) {
            row.amount = bad[i]; REQUIRE(umi_open_tms_settlement_record_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(test, "state") == 0) {
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_settlement_book_transition(&book, "payment", (UmiOpenTmsSettlementState)99, 2) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        row.state = (UmiOpenTmsSettlementState)-1;
        REQUIRE(umi_open_tms_settlement_record_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0) {
        if (strcmp(test, "count") == 0) book.count = SIZE_MAX;
        else { book.records[1] = row; book.count = 2; }
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_settlement_book_add(&book, &row) != UMI_STATUS_OK);
        REQUIRE(umi_open_tms_settlement_book_transition(&book, "payment", UMI_OPEN_TMS_SETTLEMENT_SENT, 2) != UMI_STATUS_OK);
        REQUIRE(umi_open_tms_settlement_book_measure(&book, 1001, &result) != UMI_STATUS_OK);
        REQUIRE(result.failedCount == 123 && result.overdueCount == 456);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "revision") == 0) {
        book.revision = 0; REQUIRE(umi_open_tms_settlement_book_validate(&book) == UMI_STATUS_INVALID_STATE);
        book.revision = UINT64_MAX; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_settlement_book_add(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_open_tms_settlement_book_transition(&book, "payment", UMI_OPEN_TMS_SETTLEMENT_SENT, 2) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "stale") == 0) {
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_settlement_book_transition(&book, "payment", UMI_OPEN_TMS_SETTLEMENT_SENT, 1) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "terminal") == 0) {
        REQUIRE(umi_open_tms_settlement_book_transition(&book, "payment", UMI_OPEN_TMS_SETTLEMENT_SETTLED, 2) == UMI_STATUS_OK);
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_settlement_book_transition(&book, "payment", UMI_OPEN_TMS_SETTLEMENT_PENDING, 3) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        REQUIRE(umi_open_tms_settlement_book_measure(&book, UINT64_MAX, &result) == UMI_STATUS_OK && result.overdueCount == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_OPEN_TMS_SETTLEMENT_CAPACITY; ++i) {
            book.records[i] = row; snprintf(book.records[i].id, 64, "payment.%zu", i);
        }
        book.count = UMI_OPEN_TMS_SETTLEMENT_CAPACITY; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_settlement_book_validate(&book) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_settlement_book_add(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "query") == 0) {
        char query[64]; memset(query, 'x', sizeof query); memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_settlement_book_transition(&book, query, UMI_OPEN_TMS_SETTLEMENT_SENT, 2) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_open_tms_settlement_book_measure(NULL, 0, &result) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_open_tms_settlement_book_measure(&book, 0, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(result.failedCount == 123 && result.overdueCount == 456);
    } else return 2;
    return 0;
}
