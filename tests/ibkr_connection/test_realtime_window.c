/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_window.c
 * PURPOSE: Check chronological retention, duplicates, gaps, error retirement and freshness.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "realtime_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {"rolling",
                                        "gap",
                                        "duplicate",
                                        "duplicate-stale",
                                        "conflict",
                                        "out-of-order",
                                        "received-overflow",
                                        "duplicate-overflow",
                                        "gap-overflow",
                                        "age-boundary",
                                        "age",
                                        "no-bars",
                                        "cancel-retained",
                                        "disconnect-retained",
                                        "bust",
                                        "provider-error",
                                        "reset",
                                        "independent",
                                        "invalid-copy",
                                        "invalid-bar"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    StreamReferences();

    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiIbkrRealtimeQuery q = StreamQuery();
    uint32_t request;
    CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    UmiIbkrRealtimeStore *store = UmiIbkrRealtimeFind(f->c, request);
    CHECK(store);
    if (strcmp(mode, "no-bars"))
    {
        CHECK(StreamFeed(f, request, 1791363600U) == 0);
        CHECK(StreamPump(f, 11U) == 0);
    }
    if (!strcmp(mode, "rolling"))
    {
        for (uint64_t i = 1U; i < 514U; ++i)
        {
            CHECK(StreamFeed(f, request, 1791363600U + 5U * i) == 0);
            CHECK(StreamPump(f, 11U + i) == 0);
        }
        UmiIbkrRealtimeBar oldest, last;
        CHECK(UmiIbkrRealtimeBarCopy(f->c, request, 0U, &oldest) == UMI_STATUS_OK);
        CHECK(UmiIbkrRealtimeBarCopy(f->c, request, 511U, &last) == UMI_STATUS_OK);
        CHECK(oldest.timeMilliseconds == INT64_C(1791363610000));
        CHECK(last.timeMilliseconds == INT64_C(1791366165000));
        CHECK(store->snapshot.count == 512U && store->snapshot.droppedBars == 2U &&
              store->snapshot.receivedBars == 514U);
    }
    else if (!strcmp(mode, "gap") || !strcmp(mode, "gap-overflow"))
    {
        if (!strcmp(mode, "gap-overflow"))
            store->snapshot.gapEvents = UINT64_MAX;
        CHECK(StreamFeed(f, request, 1791363615U) == 0);
        UmiStatus expected = !strcmp(mode, "gap") ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == expected);
        CHECK(store->snapshot.count == (!strcmp(mode, "gap") ? 2U : 1U));
        if (!strcmp(mode, "gap"))
            CHECK(store->snapshot.gapEvents == 1U);
    }
    else if (!strcmp(mode, "duplicate") || !strcmp(mode, "duplicate-stale") ||
             !strcmp(mode, "duplicate-overflow"))
    {
        if (!strcmp(mode, "duplicate-overflow"))
            store->snapshot.duplicateBars = UINT64_MAX;
        CHECK(StreamFeed(f, request, 1791363600U) == 0);
        UmiStatus expected =
            !strcmp(mode, "duplicate-overflow") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
        CHECK(UmiIbkrConnectionPump(f->c, 112U) == expected);
        CHECK(store->snapshot.count == 1U && store->snapshot.receivedAtMilliseconds == 11U);
        if (!strcmp(mode, "duplicate-stale"))
        {
            UmiIbkrRealtimeSnapshot s;
            CHECK(UmiIbkrRealtimeCopy(f->c, request, 112U, 100U, &s) == UMI_STATUS_OK);
            CHECK(s.stale && s.duplicateBars == 1U);
        }
    }
    else if (!strcmp(mode, "conflict") || !strcmp(mode, "out-of-order"))
    {
        if (!strcmp(mode, "conflict"))
            strcpy(store->bars[0].close.reportedText, "10");
        CHECK(StreamFeed(f, request, !strcmp(mode, "out-of-order") ? 1791363595U : 1791363600U) == 0);
        CHECK(StreamPump(f, 12U) == 0);
        CHECK(store->snapshot.failed && !store->snapshot.active && store->snapshot.needsCancel &&
              store->snapshot.count == 1U);
    }
    else if (!strcmp(mode, "received-overflow"))
    {
        store->snapshot.receivedBars = UINT64_MAX;
        CHECK(StreamFeed(f, request, 1791363605U) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(store->snapshot.count == 1U);
    }
    else if (!strcmp(mode, "age") || !strcmp(mode, "age-boundary") || !strcmp(mode, "no-bars"))
    {
        UmiIbkrRealtimeSnapshot s;
        CHECK(UmiIbkrRealtimeCopy(f->c, request, !strcmp(mode, "age") ? 112U : 111U, 100U, &s) ==
              UMI_STATUS_OK);
        CHECK(s.stale == (strcmp(mode, "age-boundary") != 0));
    }
    else if (!strcmp(mode, "cancel-retained") || !strcmp(mode, "disconnect-retained"))
    {
        if (!strcmp(mode, "cancel-retained"))
            CHECK(UmiIbkrRealtimeCancel(f->c, request, 12U) == UMI_STATUS_OK);
        else
            UmiIbkrConnectionClose(f->c);
        UmiIbkrRealtimeBar bar;
        CHECK(UmiIbkrRealtimeBarCopy(f->c, request, 0U, &bar) == UMI_STATUS_OK);
        UmiIbkrRealtimeSnapshot s;
        CHECK(UmiIbkrRealtimeCopy(f->c, request, 13U, 100U, &s) == UMI_STATUS_OK);
        CHECK(s.count == 1U && s.stale && !s.active && !s.needsCancel);
    }
    else if (!strcmp(mode, "bust") || !strcmp(mode, "provider-error") || !strcmp(mode, "reset"))
    {
        char id[32];
        (void)snprintf(id, sizeof id, "%u", (unsigned)request);
        FEED(f, "4", "2", !strcmp(mode, "reset") ? "-1" : id,
             !strcmp(mode, "reset")  ? "1101"
             : !strcmp(mode, "bust") ? "10225"
                                     : "420",
             "Unavailable");
        CHECK(UmiIbkrConnectionPump(f->c, 12U) ==
              (!strcmp(mode, "reset") ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK));
        CHECK(store->snapshot.failed && !store->snapshot.active && store->snapshot.count == 1U);
    }
    else if (!strcmp(mode, "independent"))
    {
        q.contract.contractId = 124U;
        uint32_t other;
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 15010U, &other) == UMI_STATUS_OK);
        CHECK(StreamFeed(f, other, 1791363605U) == 0);
        CHECK(StreamPump(f, 15011U) == 0);
        CHECK(UmiIbkrRealtimeCancel(f->c, request, 15012U) == UMI_STATUS_OK);
        CHECK(UmiIbkrRealtimeFind(f->c, other)->snapshot.active);
    }
    else if (!strcmp(mode, "invalid-copy"))
    {
        UmiIbkrRealtimeSnapshot s = {0};
        s.count = 99U;
        CHECK(UmiIbkrRealtimeCopy(f->c, request, 10U, 100U, &s) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrRealtimeCopy(f->c, request, 12U, 0U, &s) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrRealtimeCopy(f->c, 1U, 12U, 100U, &s) == UMI_STATUS_NOT_FOUND);
        CHECK(s.count == 99U);
    }
    else if (!strcmp(mode, "invalid-bar"))
    {
        UmiIbkrRealtimeBar b = {0};
        b.tradeCount = 99;
        CHECK(UmiIbkrRealtimeBarCopy(f->c, request, SIZE_MAX, &b) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiIbkrRealtimeBarCopy(NULL, request, 0U, &b) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(b.tradeCount == 99);
    }
    Delete(f);
    return 0;
}
