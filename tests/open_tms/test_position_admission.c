/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_position_admission.c
 *
 * PURPOSE:
 *   Verify complete position admission and unchanged state on refusal.
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
    static UmiOpenTmsPositionBook book, before;
    UmiOpenTmsPosition row = PositionRow();
    UmiOpenTmsPositionMeasures result = {123, 456, 789};
    umi_open_tms_position_book_init(&book);
    REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        strcpy(row.instrumentId, "short"); row.quantity = -1; row.marketPrice = 50; row.averagePrice = 60;
        REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_position_book_measure(&book, &result) == UMI_STATUS_OK);
        REQUIRE(result.grossExposure == 250 && result.netExposure == 150 && result.unrealisedPnl == 30);
        REQUIRE(umi_open_tms_position_book_find(&book, "account", "short") == &book.positions[1]);
    } else if (strcmp(test, "text") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsPosition, accountId), offsetof(UmiOpenTmsPosition, instrumentId), offsetof(UmiOpenTmsPosition, currency)};
        const size_t lengths[] = {64, 64, 8};
        for (size_t i = 0; i < 3; ++i) {
            row = PositionRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &book, sizeof book);
            REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        }
    } else if (strcmp(test, "numbers") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsPosition, quantity), offsetof(UmiOpenTmsPosition, averagePrice), offsetof(UmiOpenTmsPosition, marketPrice), offsetof(UmiOpenTmsPosition, realisedPnl)};
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < 4; ++i) for (size_t j = 0; j < 3; ++j) {
            row = PositionRow(); memcpy((char *)&row + offsets[i], &bad[j], sizeof(double));
            REQUIRE(umi_open_tms_position_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0) {
        if (strcmp(test, "count") == 0) book.count = SIZE_MAX;
        else { book.positions[1] = row; book.count = 2; }
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_position_book_find(&book, "account", "bond") == NULL);
        REQUIRE(umi_open_tms_position_book_upsert(&book, &row) != UMI_STATUS_OK);
        REQUIRE(umi_open_tms_position_book_measure(&book, &result) != UMI_STATUS_OK);
        REQUIRE(result.grossExposure == 123 && result.netExposure == 456 && result.unrealisedPnl == 789);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "revision") == 0) {
        book.revision = 0; REQUIRE(umi_open_tms_position_book_validate(&book) == UMI_STATUS_INVALID_STATE);
        book.revision = UINT64_MAX; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "stale") == 0) {
        row.version = 0; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        row.version = 1; REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_OPEN_TMS_POSITION_CAPACITY; ++i) {
            book.positions[i] = row; snprintf(book.positions[i].instrumentId, 64, "bond.%zu", i);
        }
        book.count = UMI_OPEN_TMS_POSITION_CAPACITY; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_position_book_validate(&book) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        row = book.positions[511]; REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "query") == 0) {
        char query[64]; memset(query, 'x', sizeof query);
        REQUIRE(umi_open_tms_position_book_find(&book, query, "bond") == NULL);
        REQUIRE(umi_open_tms_position_book_find(&book, "account", query) == NULL);
    } else if (strcmp(test, "currencies") == 0) {
        strcpy(row.instrumentId, "usd"); strcpy(row.currency, "USD");
        REQUIRE(umi_open_tms_position_book_upsert(&book, &row) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_position_book_measure(&book, &result) == UMI_STATUS_INVALID_STATE);
        REQUIRE(result.grossExposure == 123 && result.netExposure == 456 && result.unrealisedPnl == 789);
    } else if (strstr(test, "overflow") != NULL) {
        row.marketPrice = DBL_MAX; row.averagePrice = DBL_MAX;
        if (strcmp(test, "spread-overflow") == 0) { row.quantity = 1; row.averagePrice = -DBL_MAX; }
        if (strcmp(test, "sum-overflow") == 0) row.quantity = 1;
        book.positions[0] = row;
        if (strcmp(test, "sum-overflow") == 0) {
            strcpy(row.instrumentId, "second"); book.positions[1] = row; book.count = 2;
        } else REQUIRE(strcmp(test, "product-overflow") == 0 || strcmp(test, "spread-overflow") == 0);
        REQUIRE(umi_open_tms_position_book_measure(&book, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(result.grossExposure == 123 && result.netExposure == 456 && result.unrealisedPnl == 789);
        REQUIRE(umi_open_tms_position_book_gross(&book) == 0);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_open_tms_position_book_measure(NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_open_tms_position_book_measure(&book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(result.grossExposure == 123 && result.netExposure == 456 && result.unrealisedPnl == 789);
    } else return 2;
    return 0;
}
