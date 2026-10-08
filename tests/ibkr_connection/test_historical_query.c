/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_historical_query.c
 * PURPOSE: Check historical request limits and explicit Gregorian UTC input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "historical_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {"valid",          "sizes",
                                        "kinds",          "null",
                                        "contract-zero",  "contract-overflow",
                                        "route-empty",    "route-long",
                                        "duration-zero",  "duration-long",
                                        "too-many-bars",  "size",
                                        "kind",           "utc",
                                        "leap",           "invalid-leap",
                                        "invalid-month",  "invalid-day",
                                        "invalid-hour",   "invalid-minute",
                                        "invalid-second", "local-zone",
                                        "end-long"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    HistoricalReferences();
    (void)New;
    (void)Connect;
    (void)Delete;
    UmiIbkrHistoricalQuery q = HistoricalQuery();
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (!strcmp(mode, "null"))
    {
        CHECK(UmiIbkrHistoricalQueryValidate(NULL) == expected);
        return 0;
    }
    if (!strcmp(mode, "sizes"))
    {
        const uint32_t sizes[] = {60U, 300U, 900U, 1800U, 3600U};
        for (size_t i = 0U; i < 5U; ++i)
        {
            q.barSeconds = sizes[i];
            CHECK(UmiIbkrHistoricalQueryValidate(&q) == UMI_STATUS_OK);
        }
        return 0;
    }
    if (!strcmp(mode, "kinds"))
    {
        for (int i = 1; i <= 4; ++i)
        {
            q.dataKind = (UmiIbkrHistoricalDataKind)i;
            CHECK(UmiIbkrHistoricalQueryValidate(&q) == UMI_STATUS_OK);
        }
        return 0;
    }
    if (!strcmp(mode, "valid"))
        expected = UMI_STATUS_OK;
    else if (!strcmp(mode, "contract-zero"))
        q.contract.contractId = 0U;
    else if (!strcmp(mode, "contract-overflow"))
        q.contract.contractId = UINT32_MAX;
    else if (!strcmp(mode, "route-empty"))
        q.contract.exchange[0] = 0;
    else if (!strcmp(mode, "route-long"))
        memset(q.contract.exchange, 'X', sizeof q.contract.exchange);
    else if (!strcmp(mode, "duration-zero"))
        q.durationSeconds = 0U;
    else if (!strcmp(mode, "duration-long"))
    {
        q.durationSeconds = 86401U;
        q.barSeconds = 3600U;
    }
    else if (!strcmp(mode, "too-many-bars"))
        q.durationSeconds = 30720U;
    else if (!strcmp(mode, "size"))
        q.barSeconds = 61U;
    else if (!strcmp(mode, "kind"))
        q.dataKind = (UmiIbkrHistoricalDataKind)0;
    else if (!strcmp(mode, "utc"))
    {
        strcpy(q.endUtc, "20261007 12:34:56 UTC");
        expected = UMI_STATUS_OK;
    }
    else if (!strcmp(mode, "leap"))
    {
        strcpy(q.endUtc, "20000229 00:00:00 UTC");
        expected = UMI_STATUS_OK;
    }
    else if (!strcmp(mode, "invalid-leap"))
        strcpy(q.endUtc, "21000229 00:00:00 UTC");
    else if (!strcmp(mode, "invalid-month"))
        strcpy(q.endUtc, "20261301 00:00:00 UTC");
    else if (!strcmp(mode, "invalid-day"))
        strcpy(q.endUtc, "20260431 00:00:00 UTC");
    else if (!strcmp(mode, "invalid-hour"))
        strcpy(q.endUtc, "20261007 24:00:00 UTC");
    else if (!strcmp(mode, "invalid-minute"))
        strcpy(q.endUtc, "20261007 00:60:00 UTC");
    else if (!strcmp(mode, "invalid-second"))
        strcpy(q.endUtc, "20261007 00:00:60 UTC");
    else if (!strcmp(mode, "local-zone"))
        strcpy(q.endUtc, "20261007 00:00:00 EST");
    else if (!strcmp(mode, "end-long"))
        memset(q.endUtc, 'X', sizeof q.endUtc);
    CHECK(UmiIbkrHistoricalQueryValidate(&q) == expected);
    return 0;
}
