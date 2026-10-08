/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_order_recovery.c
 * PURPOSE: Exercise snapshot scope, atomic request queueing and uncorrelated completion markers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "order_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    OrderFixtureReferences();
    const char *mode = argv[1];
    Fixture *f = New();
    CHECK(f);
    if (!strcmp(mode, "not-ready"))
    {
        CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_THIS_CLIENT, 0) == UMI_STATUS_INVALID_STATE);
        Delete(f);
        return 0;
    }
    CHECK(Connect(f) == 0);
    size_t before = f->c->txSize;
    if (!strcmp(mode, "invalid-scope") || !strcmp(mode, "backward"))
    {
        CHECK(UmiIbkrOrdersRequest(
                  f->c, !strcmp(mode, "invalid-scope") ? (UmiIbkrOrderScope)0 : UMI_IBKR_ORDERS_THIS_CLIENT,
                  !strcmp(mode, "backward") ? 4U : 10U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!f->c->orders.snapshot.requested && f->c->txSize == before);
    }
    else if (!strcmp(mode, "queue-full"))
    {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_THIS_CLIENT, 10) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->orders.snapshot.requested);
        f->c->txSize = before;
        CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_THIS_CLIENT, 10) == UMI_STATUS_OK);
    }
    else if (!strcmp(mode, "unsolicited-end"))
    {
        FEED(f, "53", "1");
        CHECK(UmiIbkrConnectionPump(f->c, 10) == UMI_STATUS_OK);
        CHECK(!f->c->orders.snapshot.complete);
    }
    else
    {
        bool all = !strcmp(mode, "all-wire") || !strcmp(mode, "all-clients");
        CHECK(UmiIbkrOrdersRequest(f->c, all ? UMI_IBKR_ORDERS_ALL_CLIENTS : UMI_IBKR_ORDERS_THIS_CLIENT,
                                   10) == UMI_STATUS_OK);
        if (!strcmp(mode, "wire") || !strcmp(mode, "all-wire"))
        {
            const char *expected[] = {all ? "16" : "5", "1"};
            size_t offset = f->outSize;
            CHECK(UmiIbkrConnectionPump(f->c, 11) == UMI_STATUS_OK);
            CHECK(ObserveWire(f, offset, expected, 2U) == 0);
        }
        else if (!strcmp(mode, "busy") || !strcmp(mode, "scope-change"))
        {
            CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_ALL_CLIENTS, 11) == UMI_STATUS_INVALID_STATE);
            CHECK(f->c->orders.snapshot.scope == UMI_IBKR_ORDERS_THIS_CLIENT);
        }
        else if (!strcmp(mode, "timeout") || !strcmp(mode, "late-end"))
        {
            UmiIbkrOrderRecoverySnapshot copy;
            CHECK(UmiIbkrOrdersCopy(f->c, 10010U, 15000U, &copy) == UMI_STATUS_OK);
            CHECK(copy.failed && !copy.pending && !copy.complete && copy.stale);
            if (!strcmp(mode, "late-end"))
                FEED(f, "53", "1");
            CHECK(UmiIbkrConnectionPump(f->c, 10010U) == UMI_STATUS_OK);
            CHECK(f->c->orders.snapshot.failed && !f->c->orders.snapshot.complete);
            CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_THIS_CLIENT, 10011U) ==
                  UMI_STATUS_INVALID_STATE);
        }
        else if (!strcmp(mode, "own-client") || !strcmp(mode, "all-clients"))
        {
            CHECK(OpenFeed(f, "12", "99", "800", SIZE_MAX, NULL) == 0);
            CHECK(UmiIbkrConnectionPump(f->c, 11) == UMI_STATUS_OK);
            CHECK(f->c->orders.snapshot.count == (all ? 1U : 0U));
            CHECK(f->c->orders.identity.highestObservedOrderId == 12U);
        }
        else if (!strcmp(mode, "malformed-end"))
        {
            FEED(f, "53", "2");
            CHECK(UmiIbkrConnectionPump(f->c, 11) == UMI_STATUS_PARSE_ERROR);
            CHECK(!f->c->orders.snapshot.complete);
        }
        else if (!strcmp(mode, "empty") || !strcmp(mode, "no-repeat") || !strcmp(mode, "age") ||
                 !strcmp(mode, "disconnect") || !strcmp(mode, "duplicate-end"))
        {
            FEED(f, "53", "1");
            CHECK(UmiIbkrConnectionPump(f->c, 11) == UMI_STATUS_OK);
            CHECK(f->c->orders.snapshot.complete && f->c->orders.snapshot.count == 0U);
            if (!strcmp(mode, "no-repeat"))
                CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_THIS_CLIENT, 12) ==
                      UMI_STATUS_INVALID_STATE);
            if (!strcmp(mode, "disconnect"))
                UmiIbkrConnectionClose(f->c);
            if (!strcmp(mode, "duplicate-end"))
            {
                FEED(f, "53", "1");
                CHECK(UmiIbkrConnectionPump(f->c, 12) == UMI_STATUS_OK);
                CHECK(f->c->orders.snapshot.completedAtMilliseconds == 11U);
            }
            UmiIbkrOrderRecoverySnapshot copy;
            CHECK(UmiIbkrOrdersCopy(f->c, !strcmp(mode, "age") ? 15012U : 12U, 15000U, &copy) ==
                  UMI_STATUS_OK);
            CHECK(copy.stale == (!strcmp(mode, "age") || !strcmp(mode, "disconnect")));
        }
        else
            return 2;
    }
    Delete(f);
    return 0;
}
