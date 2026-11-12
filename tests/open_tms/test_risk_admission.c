/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_risk_admission.c
 *
 * PURPOSE:
 *   Verify complete risk admission and unchanged state on refusal.
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
    static UmiOpenTmsRiskBook book, before;
    UmiOpenTmsRiskLimit row = RiskRow();
    umi_open_tms_risk_book_init(&book);
    REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        double percent = -1; size_t breaches = 99;
        REQUIRE(umi_open_tms_risk_limit_utilisation_checked(&row, &percent) == UMI_STATUS_OK && percent == 80);
        row.utilisation = 100; REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_risk_book_breach_count_checked(&book, &breaches) == UMI_STATUS_OK && breaches == 0);
        row.utilisation = 101; REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_risk_book_breach_count_checked(&book, &breaches) == UMI_STATUS_OK && breaches == 1);
    } else if (strcmp(test, "text") == 0) {
        memset(row.id, 'x', sizeof row.id); REQUIRE(umi_open_tms_risk_limit_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        row = RiskRow(); memset(row.scopeId, 'x', sizeof row.scopeId);
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "numbers") == 0) {
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < 3; ++i) {
            row = RiskRow(); row.limit = bad[i]; REQUIRE(umi_open_tms_risk_limit_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
            row = RiskRow(); row.utilisation = bad[i]; REQUIRE(umi_open_tms_risk_limit_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
        row = RiskRow(); row.limit = 0; REQUIRE(umi_open_tms_risk_limit_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        row = RiskRow(); row.hardLimit = 2; REQUIRE(umi_open_tms_risk_limit_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        row = RiskRow(); row.utilisation = -5; REQUIRE(umi_open_tms_risk_limit_validate(&row) == UMI_STATUS_OK);
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0) {
        if (strcmp(test, "count") == 0) book.count = SIZE_MAX;
        else { book.limits[1] = row; book.count = 2; }
        memcpy(&before, &book, sizeof book); size_t breaches = 99;
        REQUIRE(umi_open_tms_risk_book_find(&book, "limit") == NULL);
        REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) != UMI_STATUS_OK);
        REQUIRE(umi_open_tms_risk_book_breach_count_checked(&book, &breaches) != UMI_STATUS_OK && breaches == 99);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "revision") == 0) {
        book.revision = 0; REQUIRE(umi_open_tms_risk_book_validate(&book) == UMI_STATUS_INVALID_STATE);
        book.revision = UINT64_MAX; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "stale") == 0) {
        row.version = 0; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_OPEN_TMS_RISK_LIMIT_CAPACITY; ++i) {
            book.limits[i] = row; snprintf(book.limits[i].id, 64, "limit.%zu", i);
        }
        book.count = UMI_OPEN_TMS_RISK_LIMIT_CAPACITY; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_risk_book_validate(&book) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
        row = book.limits[255]; REQUIRE(umi_open_tms_risk_book_upsert(&book, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "query") == 0) {
        char query[64]; memset(query, 'x', sizeof query);
        REQUIRE(umi_open_tms_risk_book_find(&book, query) == NULL);
        REQUIRE(umi_open_tms_risk_book_find(&book, "") == NULL);
    } else if (strcmp(test, "overflow") == 0) {
        double percent = 77; row.limit = DBL_MIN; row.utilisation = DBL_MAX;
        REQUIRE(umi_open_tms_risk_limit_utilisation_checked(&row, &percent) == UMI_STATUS_CAPACITY_EXCEEDED && percent == 77);
        row.limit = 1;
        REQUIRE(umi_open_tms_risk_limit_utilisation_checked(&row, &percent) == UMI_STATUS_CAPACITY_EXCEEDED && percent == 77);
    } else if (strcmp(test, "null") == 0) {
        double percent = 77; size_t breaches = 99;
        REQUIRE(umi_open_tms_risk_limit_utilisation_checked(NULL, &percent) == UMI_STATUS_INVALID_ARGUMENT && percent == 77);
        REQUIRE(umi_open_tms_risk_book_breach_count_checked(NULL, &breaches) == UMI_STATUS_INVALID_ARGUMENT && breaches == 99);
        REQUIRE(umi_open_tms_risk_book_breach_count_checked(&book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
