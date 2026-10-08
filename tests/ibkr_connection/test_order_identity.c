/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_order_identity.c
 * PURPOSE: Keep a monotonic conservative order-ID hint across broker callbacks and disconnects.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "order_fixture.h"
#include <limits.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    OrderFixtureReferences();
    const char *mode = argv[1];
    Fixture *f = New();
    CHECK(f);
    UmiIbkrOrderIdentitySnapshot id;
    if (!strcmp(mode, "unconnected"))
    {
        CHECK(UmiIbkrOrderIdentityCopy(f->c, &id) == UMI_STATUS_OK);
        CHECK(id.stale && !id.brokerIdReceived);
        Delete(f);
        return 0;
    }
    CHECK(Connect(f) == 0);
    CHECK(UmiIbkrOrderIdentityCopy(f->c, &id) == UMI_STATUS_OK);
    CHECK(id.brokerIdReceived && !id.stale && id.brokerNextOrderId == 42U);
    if (!strcmp(mode, "ready"))
        CHECK(id.conservativeNextOrderId == 42U);
    else if (!strcmp(mode, "lower") || !strcmp(mode, "higher"))
    {
        FEED(f, "9", "1", !strcmp(mode, "lower") ? "12" : "77");
        CHECK(UmiIbkrConnectionPump(f->c, 10) == UMI_STATUS_OK);
        CHECK(UmiIbkrOrderIdentityCopy(f->c, &id) == UMI_STATUS_OK);
        CHECK(id.brokerNextOrderId == (!strcmp(mode, "lower") ? 12U : 77U));
        CHECK(id.conservativeNextOrderId == (!strcmp(mode, "lower") ? 42U : 77U));
    }
    else if (!strcmp(mode, "observed") || !strcmp(mode, "negative") || !strcmp(mode, "exhausted") ||
             !strcmp(mode, "other-client"))
    {
        CHECK(StatusFeed(f,
                         !strcmp(mode, "negative")    ? "-9"
                         : !strcmp(mode, "exhausted") ? "2147483647"
                                                      : "100",
                         !strcmp(mode, "other-client") ? "99" : "35", "800", "Submitted", "0", "1") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 10) == UMI_STATUS_OK);
        CHECK(UmiIbkrOrderIdentityCopy(f->c, &id) == UMI_STATUS_OK);
        CHECK(f->c->orders.snapshot.count == 0U);
        CHECK(id.observedIdReceived == (strcmp(mode, "negative") != 0));
        CHECK(id.exhausted == (!strcmp(mode, "exhausted")));
        if (!strcmp(mode, "observed") || !strcmp(mode, "other-client"))
            CHECK(id.conservativeNextOrderId == 101U);
        if (!strcmp(mode, "negative"))
            CHECK(id.conservativeNextOrderId == 42U);
    }
    else if (!strcmp(mode, "disconnect"))
    {
        UmiIbkrConnectionClose(f->c);
        CHECK(UmiIbkrOrderIdentityCopy(f->c, &id) == UMI_STATUS_OK);
        CHECK(id.stale && id.brokerNextOrderId == 42U);
    }
    else if (!strcmp(mode, "bad-next"))
    {
        FEED(f, "9", "1", "2147483648");
        CHECK(UmiIbkrConnectionPump(f->c, 10) == UMI_STATUS_PARSE_ERROR);
        CHECK(f->c->orders.identity.brokerNextOrderId == 42U);
    }
    else if (!strcmp(mode, "execution-floor"))
    {
        uint32_t request;
        CHECK(UmiIbkrExecutionsRequest(f->c, "DU123", 10, &request) == UMI_STATUS_OK);
        char number[24];
        (void)snprintf(number, sizeof number, "%u", (unsigned)request);
        FEED(f, "11", number, "120", "123", "WORKSHOP", "STK", "", "0", "", "", "LSE", "GBP", "WRK", "WRK",
             "review.01", "20261007 12:00:00 Europe/London", "DU123", "LSE", "BOT", "10", "3.25", "800", "35",
             "0", "10", "3.25", "review", "", "0", "", "1");
        CHECK(UmiIbkrConnectionPump(f->c, 11) == UMI_STATUS_OK);
        CHECK(UmiIbkrOrderIdentityCopy(f->c, &id) == UMI_STATUS_OK);
        CHECK(f->c->executions.count == 1U && id.conservativeNextOrderId == 121U);
    }
    else if (!strcmp(mode, "no-mutations"))
    {
        size_t before = f->c->txSize;
        const char *ids[] = {"3", "4", "15", "21", "58"};
        for (size_t i = 0; i < sizeof ids / sizeof ids[0]; ++i)
        {
            const char *fields[] = {ids[i], "1"};
            CHECK(UmiIbkrQueueFields(f->c, fields, 2U) == UMI_STATUS_PERMISSION_DENIED);
        }
        CHECK(f->c->txSize == before);
    }
    else
        return 2;
    Delete(f);
    return 0;
}
