/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_activity/test_query.c
 * PURPOSE: Exercise exact date input, leap years, open bounds and reference validation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiBankActivityQuery query = ActivityQuery(); UmiFinancialDate date = {2026,9,30}, before = date;
    if (strcmp(name, "date") == 0) {
        OK(UmiBankActivityDateParse("2026-10-01", &date)); CHECK(date.year == 2026 && date.month == 10 && date.day == 1);
        OK(UmiBankActivityDateParse("", &date)); CHECK(date.year == 0 && date.month == 0 && date.day == 0);
    } else if (strcmp(name, "leap") == 0) {
        const char *valid[] = {"1600-02-29","2000-02-29","2024-02-29","9999-12-31"};
        for (size_t i = 0; i < 4; ++i) OK(UmiBankActivityDateParse(valid[i], &date));
        const char *invalid[] = {"1900-02-29","2026-02-29","1599-12-31","2026-04-31"};
        for (size_t i = 0; i < 4; ++i) CHECK(UmiBankActivityDateParse(invalid[i], &date) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "syntax") == 0) {
        const char *invalid[] = {"2026-1-01","2026/10/01"," 2026-10-01","2026-10-01x","2026-00-01","2026-10-00","2026-13-01","abcd-ef-gh"};
        for (size_t i = 0; i < 8; ++i) {
            CHECK(UmiBankActivityDateParse(invalid[i], &date) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(date.year == before.year && date.month == before.month && date.day == before.day);
        }
        CHECK(UmiBankActivityDateParse(NULL, &date) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankActivityDateParse("", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "range") == 0) {
        query.fromDate = (UmiFinancialDate){2026,10,2}; query.toDate = (UmiFinancialDate){2026,10,1};
        CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
        query.toDate = query.fromDate; OK(UmiBankActivityQueryValidate(&query));
    } else if (strcmp(name, "open-bound") == 0) {
        query.toDate = (UmiFinancialDate){2026,10,1}; OK(UmiBankActivityQueryValidate(&query));
        query.fromDate = query.toDate; query.toDate = (UmiFinancialDate){0}; OK(UmiBankActivityQueryValidate(&query));
        query.toDate.month = 1; CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "reference") == 0) {
        strcpy(query.reference, "charge-ABC_1.2"); OK(UmiBankActivityQueryValidate(&query));
        strcpy(query.reference, "*charge"); CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
        memset(query.reference, 'x', sizeof query.reference); CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "account") == 0) {
        query.accountId.value[0] = '\0'; CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
        Id(&query.accountId, "sys.cash"); CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
        memset(query.accountId.value, 'x', sizeof query.accountId.value); CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "direction") == 0) {
        for (int i = 0; i < 3; ++i) { query.direction = (UmiBankActivityDirection)i; OK(UmiBankActivityQueryValidate(&query)); }
        query.direction = (UmiBankActivityDirection)3; CHECK(UmiBankActivityQueryValidate(&query) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankActivityDirectionName(query.direction) == NULL);
    } else return 2;
    return 0;
}
