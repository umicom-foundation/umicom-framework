/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_order_session.c
 * PURPOSE: Recover orders alongside other broker observations through fragmented transport.
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
    CHECK(Connect(f) == 0);
    UmiIbkrPnlSelection selection = {0};
    strcpy(selection.account, "DU123");
    uint32_t request = 0U;
    CHECK(UmiIbkrPnlSubscribe(f->c, &selection, 10, &request) == UMI_STATUS_OK);
    CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_ALL_CLIENTS, 10) == UMI_STATUS_OK);
    if (!strcmp(mode, "fragmented-read"))
        f->readStep = 1U;
    else if (!strcmp(mode, "fragmented-write"))
        f->writeStep = 1U;
    else if (strcmp(mode, "interleaved") && strcmp(mode, "disconnect") && strcmp(mode, "late-status"))
        return 2;
    char id[24];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    CHECK(OpenFeed(f, "42", "35", "800", SIZE_MAX, NULL) == 0);
    FEED(f, "94", id, "12.50", "10", "2.50");
    CHECK(StatusFeed(f, "42", "35", "800", "Submitted", "10", "6990") == 0);
    FEED(f, "53", "1");
    for (uint64_t now = 11U; now < 300U; ++now)
        CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
    CHECK(f->c->orders.snapshot.complete && f->c->orders.snapshot.count == 1U);
    CHECK(f->c->pnl[0].received && f->c->orders.rows[0].status.filled.value.coefficient == 10);
    CHECK(f->c->orders.rows[0].hasOpenOrder);
    if (!strcmp(mode, "late-status"))
    {
        CHECK(StatusFeed(f, "42", "35", "800", "Filled", "7000", "0") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 300U) == UMI_STATUS_OK);
        CHECK(!strcmp(f->c->orders.rows[0].status.status, "Filled"));
        CHECK(f->c->orders.snapshot.completedAtMilliseconds < 300U);
    }
    if (!strcmp(mode, "disconnect"))
    {
        f->eof = true;
        CHECK(UmiIbkrConnectionPump(f->c, 300U) == UMI_STATUS_IO_ERROR);
        UmiIbkrRecoveredOrder row;
        CHECK(UmiIbkrOrderCopy(f->c, 0, 300U, 15000U, &row) == UMI_STATUS_OK);
        CHECK(row.openStale && row.statusStale && row.hasOpenOrder);
    }
    Delete(f);
    return 0;
}
