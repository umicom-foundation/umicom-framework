/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_completed_capture.c
 * PURPOSE: Check completed-history request scope, completion, retirement and transport framing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completed_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    /* Reject misspelled case names so a registration cannot silently run a default. */
    static const char *const cases[] = {"not-ready",        "backward",     "unsupported", "queue-full",
                                        "unsolicited",      "no-mutations", "wire",        "all-wire",
                                        "fragmented-write", "busy",         "timeout",     "late-end",
                                        "malformed-end",    "empty",        "age",         "disconnect",
                                        "duplicate-end",    "late-row",     "no-repeat",   "invalid-copy",
                                        "fragmented-read"};
    bool registered = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(argv[1], cases[i]))
            registered = true;
    if (!registered)
        return 2;
    CompletedFixtureReferences();
    const char *mode = argv[1];
    Fixture *f = New();
    CHECK(f);
    if (!strcmp(mode, "not-ready"))
    {
        CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 0U) == UMI_STATUS_INVALID_STATE);
        Delete(f);
        return 0;
    }
    CHECK(Connect(f) == 0);
    if (!strcmp(mode, "backward"))
    {
        CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 4U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!f->c->completed.snapshot.requested);
    }
    else if (!strcmp(mode, "unsupported"))
    {
        f->c->snapshot.protocolVersion = 177;
        CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 10U) == UMI_STATUS_NOT_IMPLEMENTED);
    }
    else if (!strcmp(mode, "queue-full"))
    {
        size_t before = f->c->txSize;
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 10U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->completed.snapshot.requested);
        f->c->txSize = before;
        CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 10U) == UMI_STATUS_OK);
    }
    else if (!strcmp(mode, "unsolicited"))
    {
        CompletedFields fields = CompletedExample();
        CHECK(CompletedFeed(f, &fields) == 0);
        FEED(f, "102");
        CHECK(UmiIbkrConnectionPump(f->c, 10U) == UMI_STATUS_OK);
        CHECK(!f->c->completed.snapshot.requested && f->c->completed.snapshot.count == 0U);
    }
    else if (!strcmp(mode, "no-mutations"))
    {
        const char *ids[] = {"3", "4", "15", "21", "58"};
        for (size_t i = 0U; i < sizeof ids / sizeof ids[0]; ++i)
        {
            const char *packet[] = {ids[i], "1"};
            CHECK(UmiIbkrQueueFields(f->c, packet, 2U) == UMI_STATUS_PERMISSION_DENIED);
        }
    }
    else
    {
        bool apiOnly = strcmp(mode, "all-wire") != 0;
        CHECK(UmiIbkrCompletedOrdersRequest(f->c, apiOnly, 10U) == UMI_STATUS_OK);
        if (!strcmp(mode, "wire") || !strcmp(mode, "all-wire") || !strcmp(mode, "fragmented-write"))
        {
            if (!strcmp(mode, "fragmented-write"))
                f->writeStep = 1U;
            size_t offset = f->outSize;
            for (uint64_t now = 11U; f->c->txSize && now < 100U; ++now)
                CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
            const char *expected[] = {"99", apiOnly ? "1" : "0"};
            CHECK(ObserveWire(f, offset, expected, 2U) == 0);
        }
        else if (!strcmp(mode, "busy"))
        {
            CHECK(UmiIbkrCompletedOrdersRequest(f->c, false, 11U) == UMI_STATUS_INVALID_STATE);
            CHECK(f->c->completed.snapshot.apiOnly);
        }
        else if (!strcmp(mode, "timeout") || !strcmp(mode, "late-end"))
        {
            UmiIbkrCompletedOrdersSnapshot copy;
            CHECK(UmiIbkrCompletedOrdersCopy(f->c, 10010U, 15000U, &copy) == UMI_STATUS_OK);
            CHECK(copy.failed && copy.stale && !copy.pending && !copy.complete);
            if (!strcmp(mode, "late-end"))
                FEED(f, "102");
            CHECK(UmiIbkrConnectionPump(f->c, 10010U) == UMI_STATUS_OK);
            CHECK(!f->c->completed.snapshot.complete);
            CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 10011U) == UMI_STATUS_INVALID_STATE);
        }
        else if (!strcmp(mode, "malformed-end"))
        {
            FEED(f, "102", "1");
            CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_PARSE_ERROR);
            CHECK(!f->c->completed.snapshot.complete);
        }
        else if (!strcmp(mode, "empty") || !strcmp(mode, "age") || !strcmp(mode, "disconnect") ||
                 !strcmp(mode, "duplicate-end") || !strcmp(mode, "late-row") || !strcmp(mode, "no-repeat") ||
                 !strcmp(mode, "invalid-copy"))
        {
            FEED(f, "102");
            CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
            if (!strcmp(mode, "duplicate-end"))
            {
                FEED(f, "102");
                CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
                CHECK(f->c->completed.snapshot.completedAtMilliseconds == 11U);
            }
            if (!strcmp(mode, "late-row"))
            {
                CompletedFields fields = CompletedExample();
                CHECK(CompletedFeed(f, &fields) == 0);
                CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
                CHECK(f->c->completed.snapshot.count == 0U);
            }
            if (!strcmp(mode, "no-repeat"))
                CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 12U) == UMI_STATUS_INVALID_STATE);
            if (!strcmp(mode, "disconnect"))
                UmiIbkrConnectionClose(f->c);
            UmiIbkrCompletedOrdersSnapshot copy;
            CHECK(UmiIbkrCompletedOrdersCopy(f->c, !strcmp(mode, "age") ? 15012U : 12U, 15000U, &copy) ==
                  UMI_STATUS_OK);
            CHECK(copy.complete && copy.count == 0U);
            CHECK(copy.stale == (!strcmp(mode, "age") || !strcmp(mode, "disconnect")));
            if (!strcmp(mode, "invalid-copy"))
            {
                copy.count = 999U;
                CHECK(UmiIbkrCompletedOrdersCopy(f->c, 12U, 0U, &copy) == UMI_STATUS_INVALID_ARGUMENT);
                CHECK(copy.count == 999U);
            }
        }
        else if (!strcmp(mode, "fragmented-read"))
        {
            f->readStep = 1U;
            CompletedFields fields = CompletedExample();
            CHECK(CompletedFeed(f, &fields) == 0);
            FEED(f, "102");
            CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
            CHECK(f->c->completed.snapshot.complete && f->c->completed.snapshot.count == 1U);
        }
        else
            return 2;
    }
    Delete(f);
    return 0;
}
