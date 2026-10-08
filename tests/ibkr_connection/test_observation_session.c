/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_observation_session.c
 * PURPOSE: Check mixed broker observations over one fragmented transport without cross-request contamination.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static int Run(Fixture *f, const char *mode)
{
    CHECK(Connect(f) == 0);
    UmiIbkrPnlSelection account = {0};
    strcpy(account.account, "DU123");
    UmiIbkrDepthSelection book = {0};
    book.contractId = 123U;
    book.rows = 5U;
    strcpy(book.exchange, "LSE");
    uint32_t pnl = 0U, search = 0U, depth = 0U;
    CHECK(UmiIbkrPnlSubscribe(f->c, &account, 10U, &pnl) == UMI_STATUS_OK);
    CHECK(UmiIbkrSymbolSearchRequest(f->c, "Workshop", 10U, &search) == UMI_STATUS_OK);
    CHECK(UmiIbkrDepthSubscribe(f->c, &book, 10U, &depth) == UMI_STATUS_OK);
    CHECK(pnl == 36000U && search == 36001U && depth == 36002U);
    if (!strcmp(mode, "no-orders"))
    {
        const char *place[] = {"3", "1"}, *cancel[] = {"4", "1"}, *exercise[] = {"21", "1"},
                   *cancelAll[] = {"58", "1"};
        size_t before = f->c->txSize;
        CHECK(UmiIbkrQueueFields(f->c, place, 2U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiIbkrQueueFields(f->c, cancel, 2U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiIbkrQueueFields(f->c, exercise, 2U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiIbkrQueueFields(f->c, cancelAll, 2U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(f->c->txSize == before);
        return 0;
    }
    if (!strcmp(mode, "cancel-one"))
        CHECK(UmiIbkrPnlCancel(f->c, pnl) == UMI_STATUS_OK);
    if (!strcmp(mode, "fragmented-read"))
        f->readStep = 1U;
    if (!strcmp(mode, "fragmented-write"))
        f->writeStep = 1U;
    /* Interleave response families in an order different from their requests.
     * Request ownership, not arrival order, selects the destination snapshot. */
    FEED(f, "12", "1", "36002", "0", "0", "1", "125", "3");
    FEED(f, "94", "36000", "1.25", "-2", "0");
    FEED(f, "79", "36001", "0");
    for (uint64_t now = 11U; now < 200U && (f->inAt < f->inSize || f->c->txSize != 0U || f->c->rxSize != 0U);
         ++now)
        CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
    CHECK(f->inAt == f->inSize && f->c->rxSize == 0U && f->c->txSize == 0U);
    CHECK(f->c->pnl[0].received == (strcmp(mode, "cancel-one") != 0));
    CHECK(f->c->symbolSearch.complete && !f->c->symbolSearch.failed && f->c->symbolSearch.count == 0U);
    CHECK(f->c->depth[0].bidCount == 1U && f->c->depth[0].askCount == 0U);
    CHECK(f->c->depth[0].bids[0].price.coefficient == 125);
    if (!strcmp(mode, "depth-reset"))
    {
        FEED(f, "4", "2", "36002", "317", "Reset book");
        CHECK(UmiIbkrConnectionPump(f->c, 200U) == UMI_STATUS_OK);
        CHECK(f->c->depth[0].bidCount == 0U && f->c->depth[0].resetCount == 1U);
        CHECK(f->c->pnl[0].received && !f->c->pnl[0].failed && f->c->symbolSearch.complete);
    }
    if (!strcmp(mode, "replaced-search"))
    {
        CHECK(UmiIbkrSymbolSearchRequest(f->c, "Other", 1010U, &search) == UMI_STATUS_OK && search == 36003U);
        FEED(f, "79", "36001", "0");
        CHECK(UmiIbkrConnectionPump(f->c, 1011U) == UMI_STATUS_OK);
        CHECK(!f->c->symbolSearch.complete && f->c->pnl[0].received && f->c->depth[0].received);
    }
    if (!strcmp(mode, "disconnect"))
    {
        UmiIbkrConnectionClose(f->c);
        UmiIbkrPnlSnapshot pnlCopy;
        UmiIbkrDepthSnapshot *depthCopy = malloc(sizeof *depthCopy);
        UmiIbkrSymbolSearchSnapshot *searchCopy = malloc(sizeof *searchCopy);
        CHECK(depthCopy && searchCopy);
        CHECK(UmiIbkrPnlCopy(f->c, pnl, 200U, 1000U, &pnlCopy) == UMI_STATUS_OK && pnlCopy.stale);
        CHECK(UmiIbkrDepthCopy(f->c, depth, 200U, 1000U, depthCopy) == UMI_STATUS_OK && depthCopy->stale);
        CHECK(UmiIbkrSymbolSearchCopy(f->c, search, 200U, searchCopy) == UMI_STATUS_OK && searchCopy->stale);
        free(depthCopy);
        free(searchCopy);
    }
    return 0;
}
int main(int argc, char **argv)
{
    (void)PositionFeed;
    if (argc != 2)
        return 2;
    Fixture *f = New();
    if (f == NULL)
        return 1;
    int result = Run(f, argv[1]);
    Delete(f);
    return result;
}
