/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_query.c
 * PURPOSE: Validate streaming subscription intent before any packet is queued.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "realtime_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {
        "valid",       "kinds",      "null",          "zero-contract", "overflow-contract",
        "empty-route", "long-route", "control-route", "bad-kind"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    StreamReferences();

    UmiIbkrRealtimeQuery q = StreamQuery();
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (!strcmp(mode, "valid"))
        expected = UMI_STATUS_OK;
    else if (!strcmp(mode, "kinds"))
    {
        for (int kind = 1; kind <= 4; ++kind)
        {
            q.dataKind = (UmiIbkrHistoricalDataKind)kind;
            CHECK(UmiIbkrRealtimeQueryValidate(&q) == UMI_STATUS_OK);
        }
        return 0;
    }
    else if (!strcmp(mode, "zero-contract"))
        q.contract.contractId = 0;
    else if (!strcmp(mode, "overflow-contract"))
        q.contract.contractId = UINT32_MAX;
    else if (!strcmp(mode, "empty-route"))
        q.contract.exchange[0] = 0;
    else if (!strcmp(mode, "long-route"))
        memset(q.contract.exchange, 'a', sizeof q.contract.exchange);
    else if (!strcmp(mode, "control-route"))
        strcpy(q.contract.exchange, "S\n");
    else if (!strcmp(mode, "bad-kind"))
        q.dataKind = (UmiIbkrHistoricalDataKind)0;
    CHECK(UmiIbkrRealtimeQueryValidate(!strcmp(mode, "null") ? NULL : &q) == expected);
    return 0;
}
