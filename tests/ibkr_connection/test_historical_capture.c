/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_historical_capture.c
 * PURPOSE: Exercise request publication, wire framing, cancellation, pacing and retained capture state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "historical_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {"disconnect-pending",
                                        "not-ready",
                                        "backward",
                                        "unsupported",
                                        "queue-full",
                                        "no-mutations",
                                        "wire",
                                        "fragmented-write",
                                        "busy",
                                        "cancel",
                                        "cancel-full",
                                        "late-cancel",
                                        "pacing",
                                        "replace",
                                        "replace-full",
                                        "timeout",
                                        "late-timeout",
                                        "provider-error",
                                        "empty",
                                        "age",
                                        "disconnect",
                                        "invalid-copy",
                                        "fragmented-read",
                                        "request-overflow",
                                        "old-request"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    HistoricalReferences();
    Fixture *f = New();
    CHECK(f);
    UmiIbkrHistoricalQuery q = HistoricalQuery();
    uint32_t request = 999U;
    if (!strcmp(mode, "not-ready"))
    {
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 0U, &request) == UMI_STATUS_INVALID_STATE);
        CHECK(request == 999U);
        Delete(f);
        return 0;
    }
    CHECK(Connect(f) == 0);
    if (!strcmp(mode, "backward"))
    {
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 4U, &request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!f->c->history && request == 999U);
    }
    else if (!strcmp(mode, "unsupported"))
    {
        f->c->snapshot.protocolVersion = 177;
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_NOT_IMPLEMENTED);
    }
    else if (!strcmp(mode, "disconnect-pending"))
    {
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
        UmiIbkrConnectionClose(f->c);
        UmiIbkrHistoricalSnapshot capture;
        CHECK(UmiIbkrHistoricalCopy(f->c, request, 11U, 100U, &capture) == UMI_STATUS_OK);
        CHECK(capture.failed && capture.stale && !capture.pending && !capture.complete);
    }
    else if (!strcmp(mode, "queue-full"))
    {
        size_t before = f->c->txSize;
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->history && !f->c->historyRequested && request == 999U);
        f->c->txSize = before;
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    }
    else if (!strcmp(mode, "request-overflow"))
    {
        f->c->nextQuoteRequest = INT32_MAX;
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->history);
    }
    else if (!strcmp(mode, "no-mutations"))
    {
        const char *ids[] = {"3", "4", "15", "21", "58"};
        for (size_t i = 0U; i < 5U; ++i)
        {
            const char *packet[] = {ids[i], "1"};
            CHECK(UmiIbkrQueueFields(f->c, packet, 2U) == UMI_STATUS_PERMISSION_DENIED);
        }
    }
    else
    {
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
        if (!strcmp(mode, "wire") || !strcmp(mode, "fragmented-write"))
        {
            if (!strcmp(mode, "fragmented-write"))
                f->writeStep = 1U;
            size_t offset = f->outSize;
            for (uint64_t now = 11U; f->c->txSize && now < 1000U; ++now)
                CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *expected[] = {"20",    id,        "123", "",       "",  "",  "0", "",
                                      "",      "SMART",   "",    "",       "",  "",  "0", "",
                                      "1 min", "14400 S", "1",   "TRADES", "2", "0", ""};
            CHECK(ObserveWire(f, offset, expected, 23U) == 0);
        }
        else if (!strcmp(mode, "busy"))
        {
            CHECK(UmiIbkrHistoricalRequest(f->c, &q, 11U, &request) == UMI_STATUS_BUSY);
        }
        else if (!strcmp(mode, "cancel") || !strcmp(mode, "cancel-full") || !strcmp(mode, "late-cancel") ||
                 !strcmp(mode, "pacing"))
        {
            CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
            size_t offset = f->outSize;
            if (!strcmp(mode, "cancel-full"))
            {
                f->c->txSize = UMI_IBKR_TX_LIMIT;
                CHECK(UmiIbkrHistoricalCancel(f->c, request, 12U) == UMI_STATUS_CAPACITY_EXCEEDED);
                CHECK(f->c->history->snapshot.pending);
                f->c->txSize = 0U;
            }
            CHECK(UmiIbkrHistoricalCancel(f->c, request, 12U) == UMI_STATUS_OK);
            CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *expected[] = {"25", "1", id};
            CHECK(ObserveWire(f, offset, expected, 3U) == 0);
            CHECK(f->c->history->snapshot.cancelled && !f->c->history->snapshot.pending);
            if (!strcmp(mode, "late-cancel"))
            {
                CHECK(HistoricalFeed(f, request) == 0);
                CHECK(UmiIbkrConnectionPump(f->c, 14U) == UMI_STATUS_OK);
                CHECK(f->c->history->snapshot.count == 0U);
            }
            if (!strcmp(mode, "pacing"))
            {
                uint32_t next = 0U;
                CHECK(UmiIbkrHistoricalRequest(f->c, &q, 15009U, &next) == UMI_STATUS_BUSY);
                CHECK(next == 0U);
                CHECK(UmiIbkrHistoricalRequest(f->c, &q, 15010U, &next) == UMI_STATUS_OK);
                CHECK(next > request);
            }
        }
        else if (!strcmp(mode, "timeout") || !strcmp(mode, "late-timeout"))
        {
            UmiIbkrHistoricalSnapshot s;
            CHECK(UmiIbkrHistoricalCopy(f->c, request, 60010U, 100U, &s) == UMI_STATUS_OK);
            CHECK(s.failed && !s.pending && s.stale);
            if (!strcmp(mode, "late-timeout"))
                CHECK(HistoricalFeed(f, request) == 0);
            CHECK(UmiIbkrConnectionPump(f->c, 60010U) == UMI_STATUS_OK);
            CHECK(!f->c->history->snapshot.complete);
        }
        else if (!strcmp(mode, "provider-error"))
        {
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            FEED(f, "4", "2", id, "162", "Historical data not available");
            CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
            CHECK(f->c->history->snapshot.failed && f->c->history->snapshot.providerCode == 162);
        }
        else
        {
            if (!strcmp(mode, "empty"))
            {
                char id[32];
                (void)snprintf(id, sizeof id, "%u", (unsigned)request);
                FEED(f, "17", id, "", "", "0");
            }
            else
                CHECK(HistoricalFeed(f, request) == 0);
            if (!strcmp(mode, "fragmented-read"))
                f->readStep = 1U;
            uint64_t now = 11U;
            do
            {
                CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
                ++now;
            } while (f->inAt < f->inSize && now < 1000U);
            CHECK(f->c->history->snapshot.complete);
            CHECK(f->c->history->snapshot.count == (!strcmp(mode, "empty") ? 0U : 2U));
            if (!strcmp(mode, "disconnect"))
                UmiIbkrConnectionClose(f->c);
            UmiIbkrHistoricalSnapshot snapshot;
            CHECK(UmiIbkrHistoricalCopy(f->c, request, !strcmp(mode, "age") ? 1001U : now, 100U, &snapshot) ==
                  UMI_STATUS_OK);
            CHECK(snapshot.stale == (!strcmp(mode, "age") || !strcmp(mode, "disconnect")));
            if (!strcmp(mode, "invalid-copy"))
            {
                snapshot.count = 99U;
                CHECK(UmiIbkrHistoricalCopy(f->c, request, now, 0U, &snapshot) ==
                      UMI_STATUS_INVALID_ARGUMENT);
                CHECK(snapshot.count == 99U);
            }
            if (!strcmp(mode, "replace") || !strcmp(mode, "replace-full") || !strcmp(mode, "old-request"))
            {
                uint32_t next = 0U;
                if (!strcmp(mode, "replace-full"))
                {
                    f->c->txSize = UMI_IBKR_TX_LIMIT;
                    CHECK(UmiIbkrHistoricalRequest(f->c, &q, 15010U, &next) == UMI_STATUS_CAPACITY_EXCEEDED);
                    CHECK(f->c->history->snapshot.complete && f->c->history->snapshot.requestId == request);
                    f->c->txSize = 0U;
                }
                CHECK(UmiIbkrHistoricalRequest(f->c, &q, 15010U, &next) == UMI_STATUS_OK);
                CHECK(next > request && f->c->history->snapshot.count == 0U);
                if (!strcmp(mode, "old-request"))
                {
                    CHECK(HistoricalFeed(f, request) == 0);
                    CHECK(UmiIbkrConnectionPump(f->c, 15011U) == UMI_STATUS_OK);
                    CHECK(f->c->history->snapshot.pending && f->c->history->snapshot.count == 0U);
                }
            }
        }
    }
    Delete(f);
    return 0;
}
