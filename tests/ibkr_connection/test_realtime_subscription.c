/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_subscription.c
 * PURPOSE: Check request and cancellation framing, pacing, queue rollback and slot ownership.
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
        "not-ready",        "backward",      "unsupported",       "null-output",  "wire",
        "midpoint-wire",    "outside-hours", "fragmented-write",  "no-mutations", "queue-full",
        "request-overflow", "cancel",        "cancel-full",       "late-cancel",  "pacing",
        "shared-history",   "shared-stream", "duplicate",         "capacity",     "reuse",
        "old-request",      "failed-cancel", "close-before-first"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    StreamReferences();

    Fixture *f = New();
    CHECK(f);
    UmiIbkrRealtimeQuery q = StreamQuery();
    uint32_t request = 999U;
    if (!strcmp(mode, "not-ready"))
    {
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 0U, &request) == UMI_STATUS_INVALID_STATE);
        CHECK(request == 999U);
        Delete(f);
        return 0;
    }
    CHECK(Connect(f) == 0);
    if (!strcmp(mode, "backward") || !strcmp(mode, "null-output"))
    {
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, !strcmp(mode, "backward") ? 4U : 10U,
                                     !strcmp(mode, "null-output") ? NULL : &request) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(request == 999U);
    }
    else if (!strcmp(mode, "unsupported"))
    {
        f->c->snapshot.protocolVersion = 177;
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_NOT_IMPLEMENTED);
    }
    else if (!strcmp(mode, "queue-full"))
    {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->realtime[0] && !f->c->barRequested && request == 999U);
        f->c->txSize = 0;
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    }
    else if (!strcmp(mode, "request-overflow"))
    {
        f->c->nextQuoteRequest = INT32_MAX;
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->realtime[0]);
    }
    else if (!strcmp(mode, "no-mutations"))
    {
        const char *ids[] = {"3", "4", "15", "21", "58"};
        for (size_t i = 0; i < 5U; ++i)
        {
            const char *fields[] = {ids[i], "1"};
            CHECK(UmiIbkrQueueFields(f->c, fields, 2U) == UMI_STATUS_PERMISSION_DENIED);
        }
    }
    else if (!strcmp(mode, "shared-history"))
    {
        UmiIbkrHistoricalQuery h = HistoricalQuery();
        uint32_t history;
        CHECK(UmiIbkrHistoricalRequest(f->c, &h, 10U, &history) == UMI_STATUS_OK);
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 15009U, &request) == UMI_STATUS_BUSY);
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 15010U, &request) == UMI_STATUS_OK);
        CHECK(request != history);
    }
    else
    {
        if (!strcmp(mode, "midpoint-wire"))
            q.dataKind = UMI_IBKR_HISTORY_MIDPOINT;
        if (!strcmp(mode, "outside-hours"))
            q.regularHours = false;
        CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
        UmiIbkrRealtimeStore *store = UmiIbkrRealtimeFind(f->c, request);
        CHECK(store);
        if (strstr(mode, "wire") || !strcmp(mode, "fragmented-write") || !strcmp(mode, "outside-hours"))
        {
            size_t offset = f->outSize;
            if (!strcmp(mode, "fragmented-write"))
                f->writeStep = 1U;
            for (uint64_t now = 11U; f->c->txSize && now < 1000U; ++now)
                CHECK(StreamPump(f, now) == 0);
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *expected[] = {"50",
                                      "3",
                                      id,
                                      "123",
                                      "",
                                      "",
                                      "",
                                      "0",
                                      "",
                                      "",
                                      "SMART",
                                      "",
                                      "",
                                      "",
                                      "",
                                      "5",
                                      !strcmp(mode, "midpoint-wire") ? "MIDPOINT" : "TRADES",
                                      !strcmp(mode, "outside-hours") ? "0" : "1",
                                      ""};
            CHECK(ObserveWire(f, offset, expected, 19U) == 0);
        }
        else if (!strcmp(mode, "shared-stream"))
        {
            UmiIbkrHistoricalQuery h = HistoricalQuery();
            uint32_t history = 0;
            CHECK(UmiIbkrHistoricalRequest(f->c, &h, 15009U, &history) == UMI_STATUS_BUSY);
            CHECK(UmiIbkrHistoricalRequest(f->c, &h, 15010U, &history) == UMI_STATUS_OK);
            CHECK(history != request);
        }
        else if (!strcmp(mode, "duplicate"))
        {
            uint32_t next = 0;
            CHECK(UmiIbkrRealtimeRequest(f->c, &q, 15010U, &next) == UMI_STATUS_BUSY);
            CHECK(!next && store->snapshot.active);
        }
        else if (!strcmp(mode, "capacity"))
        {
            for (uint32_t i = 1; i < 4U; ++i)
            {
                uint32_t next = 0;
                q.contract.contractId = 123U + i;
                CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U + 15000U * i, &next) == UMI_STATUS_OK);
            }
            q.contract.contractId = 999U;
            uint32_t next = 0;
            CHECK(UmiIbkrRealtimeRequest(f->c, &q, 60010U, &next) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(!next);
        }
        else if (!strcmp(mode, "close-before-first"))
        {
            UmiIbkrConnectionClose(f->c);
            CHECK(!store->snapshot.active && store->snapshot.failed && !store->snapshot.needsCancel);
        }
        else
        {
            CHECK(StreamPump(f, 11U) == 0);
            size_t offset = f->outSize;
            if (!strcmp(mode, "cancel-full"))
            {
                f->c->txSize = UMI_IBKR_TX_LIMIT;
                CHECK(UmiIbkrRealtimeCancel(f->c, request, 12U) == UMI_STATUS_CAPACITY_EXCEEDED);
                CHECK(store->snapshot.active && store->snapshot.needsCancel);
                f->c->txSize = 0U;
            }
            if (!strcmp(mode, "failed-cancel"))
            {
                char id[32];
                (void)snprintf(id, sizeof id, "%u", (unsigned)request);
                FEED(f, "4", "2", id, "10225", "Bust");
                CHECK(StreamPump(f, 12U) == 0);
                CHECK(store->snapshot.failed && store->snapshot.needsCancel);
            }
            CHECK(UmiIbkrRealtimeCancel(f->c, request, 13U) == UMI_STATUS_OK);
            CHECK(StreamPump(f, 14U) == 0);
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *expected[] = {"51", "1", id};
            CHECK(ObserveWire(f, offset, expected, 3U) == 0);
            CHECK(store->snapshot.cancelled && !store->snapshot.active && !store->snapshot.needsCancel);
            if (!strcmp(mode, "late-cancel"))
            {
                CHECK(StreamFeed(f, request, 1791363600U) == 0);
                CHECK(StreamPump(f, 15U) == 0);
                CHECK(store->snapshot.count == 0U);
            }
            if (!strcmp(mode, "pacing") || !strcmp(mode, "reuse") || !strcmp(mode, "old-request"))
            {
                uint32_t next = 0;
                CHECK(UmiIbkrRealtimeRequest(f->c, &q, 15009U, &next) == UMI_STATUS_BUSY);
                CHECK(UmiIbkrRealtimeRequest(f->c, &q, 15010U, &next) == UMI_STATUS_OK);
                CHECK(next > request && !UmiIbkrRealtimeFind(f->c, request));
                if (!strcmp(mode, "old-request"))
                {
                    CHECK(StreamFeed(f, request, 1791363600U) == 0);
                    CHECK(StreamPump(f, 15011U) == 0);
                    CHECK(UmiIbkrRealtimeFind(f->c, next)->snapshot.count == 0U);
                }
            }
        }
    }
    Delete(f);
    return 0;
}
