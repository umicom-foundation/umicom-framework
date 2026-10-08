/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_market_depth.c
 * PURPOSE: Exercise depth framing, positional edits, reset recovery and stream isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_fixture.h"
#include <limits.h>
static int Delta(Fixture *f, const char *request, const char *position, const char *operation,
                 const char *side, const char *price, const char *size, const char *maker, const char *smart)
{
    if (maker != NULL)
    {
        FEED(f, "13", "1", request, position, maker, operation, side, price, size, smart);
    }
    else
    {
        FEED(f, "12", "1", request, position, operation, side, price, size);
    }
    return 0;
}
static int Run(Fixture *f, const char *mode, UmiIbkrDepthSnapshot *copy)
{
    CHECK(Connect(f) == 0);
    UmiIbkrDepthSelection selection = {0};
    selection.contractId = 123U;
    selection.rows = 2U;
    strcpy(selection.exchange, "LSE");
    uint32_t request = 99U;
    bool smart = !strcmp(mode, "smart") || !strcmp(mode, "smart-wire") || !strcmp(mode, "smart-cancel");
    if (smart)
    {
        strcpy(selection.exchange, "SMART");
        selection.smartDepth = true;
    }
    if (!strcmp(mode, "zero-id") || !strcmp(mode, "zero-rows") || !strcmp(mode, "large-rows") ||
        !strcmp(mode, "missing-exchange") || !strcmp(mode, "route-mismatch") || !strcmp(mode, "combo") ||
        !strcmp(mode, "bad-strike") || !strcmp(mode, "bad-multiplier") || !strcmp(mode, "bad-right") ||
        !strcmp(mode, "unterminated") || !strcmp(mode, "invalid-utf8"))
    {
        if (!strcmp(mode, "zero-id"))
            selection.contractId = 0U;
        if (!strcmp(mode, "zero-rows"))
            selection.rows = 0U;
        if (!strcmp(mode, "large-rows"))
            selection.rows = UMI_IBKR_DEPTH_ROW_LIMIT + 1U;
        if (!strcmp(mode, "missing-exchange"))
            selection.exchange[0] = '\0';
        if (!strcmp(mode, "route-mismatch"))
            selection.smartDepth = true;
        if (!strcmp(mode, "combo"))
            strcpy(selection.securityType, "BAG");
        if (!strcmp(mode, "bad-strike"))
            strcpy(selection.strike, "-1");
        if (!strcmp(mode, "bad-multiplier"))
            strcpy(selection.multiplier, "0");
        if (!strcmp(mode, "bad-right"))
            strcpy(selection.right, "CALL");
        if (!strcmp(mode, "unterminated"))
            memset(selection.symbol, 'x', sizeof selection.symbol);
        if (!strcmp(mode, "invalid-utf8"))
            strcpy(selection.symbol, "\xc0\xaf");
        CHECK(UmiIbkrDepthSubscribe(f->c, &selection, 10U, &request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(request == 99U && f->c->depth[0].requestId == 0U);
        return 0;
    }
    if (!strcmp(mode, "queue-full") || !strcmp(mode, "not-ready") || !strcmp(mode, "unsupported") ||
        !strcmp(mode, "id-exhausted"))
    {
        UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (!strcmp(mode, "queue-full"))
            f->c->txSize = UMI_IBKR_TX_LIMIT;
        if (!strcmp(mode, "id-exhausted"))
            f->c->nextQuoteRequest = INT_MAX;
        if (!strcmp(mode, "not-ready"))
        {
            f->c->snapshot.state = UMI_IBKR_WAITING;
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (!strcmp(mode, "unsupported"))
        {
            f->c->snapshot.protocolVersion = 177;
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        }
        CHECK(UmiIbkrDepthSubscribe(f->c, &selection, 10U, &request) == expected);
        CHECK(request == 99U && f->c->depth[0].requestId == 0U);
        return 0;
    }
    if (!strcmp(mode, "descriptors"))
    {
        strcpy(selection.symbol, "WORKSHOP");
        strcpy(selection.securityType, "OPT");
        strcpy(selection.expiry, "202612");
        strcpy(selection.strike, "125.5");
        strcpy(selection.right, "C");
        strcpy(selection.multiplier, "100");
        strcpy(selection.primaryExchange, "LSE");
        strcpy(selection.currency, "GBP");
        strcpy(selection.localSymbol, "WORKSHOP OPT");
        strcpy(selection.tradingClass, "WORKSHOP");
    }
    size_t before = f->outSize;
    CHECK(UmiIbkrDepthSubscribe(f->c, &selection, 10U, &request) == UMI_STATUS_OK && request == 36000U);
    if (!strcmp(mode, "wire") || !strcmp(mode, "smart-wire") || !strcmp(mode, "descriptors"))
    {
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        bool described = !strcmp(mode, "descriptors");
        const char *fields[] = {"10",
                                "5",
                                "36000",
                                "123",
                                described ? "WORKSHOP" : "",
                                described ? "OPT" : "",
                                described ? "202612" : "",
                                described ? "125.5" : "0",
                                described ? "C" : "",
                                described ? "100" : "",
                                smart ? "SMART" : "LSE",
                                described ? "LSE" : "",
                                described ? "GBP" : "",
                                described ? "WORKSHOP OPT" : "",
                                described ? "WORKSHOP" : "",
                                "2",
                                smart ? "1" : "0",
                                ""};
        CHECK(ObserveWire(f, before, fields, sizeof fields / sizeof fields[0]) == 0);
        return 0;
    }
    if (!strcmp(mode, "duplicate"))
    {
        uint32_t other = 99U;
        CHECK(UmiIbkrDepthSubscribe(f->c, &selection, 11U, &other) == UMI_STATUS_ALREADY_EXISTS &&
              other == 99U);
        return 0;
    }
    if (!strcmp(mode, "capacity"))
    {
        for (uint32_t i = 1U; i < UMI_IBKR_DEPTH_STREAM_LIMIT; ++i)
        {
            selection.contractId = 123U + i;
            CHECK(UmiIbkrDepthSubscribe(f->c, &selection, 10U, &request) == UMI_STATUS_OK);
        }
        selection.contractId = 200U;
        request = 99U;
        CHECK(UmiIbkrDepthSubscribe(f->c, &selection, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED &&
              request == 99U);
        return 0;
    }
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    if (!strcmp(mode, "cancel-wire") || !strcmp(mode, "smart-cancel") || !strcmp(mode, "cancel-full"))
    {
        if (!strcmp(mode, "cancel-full"))
        {
            f->c->txSize = UMI_IBKR_TX_LIMIT;
            CHECK(UmiIbkrDepthCancel(f->c, request) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(UmiIbkrDepthCopy(f->c, request, 11U, 100U, copy) == UMI_STATUS_OK && copy->subscribed);
            return 0;
        }
        before = f->outSize;
        CHECK(UmiIbkrDepthCancel(f->c, request) == UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
        const char *fields[] = {"11", "1", "36000", smart ? "1" : "0"};
        CHECK(ObserveWire(f, before, fields, 4U) == 0);
        CHECK(UmiIbkrDepthCancel(f->c, request) == UMI_STATUS_OK && f->c->txSize == 0U);
        return 0;
    }
    if (!strcmp(mode, "alias-reuse"))
    {
        CHECK(UmiIbkrDepthCancel(f->c, request) == UMI_STATUS_OK);
        CHECK(UmiIbkrDepthSubscribe(f->c, &f->c->depth[0].selection, 12U, &request) == UMI_STATUS_OK &&
              request == 36001U);
        CHECK(f->c->depth[0].selection.contractId == 123U &&
              !strcmp(f->c->depth[0].selection.exchange, "LSE"));
        return 0;
    }
    if (!strcmp(mode, "waiting"))
    {
        CHECK(UmiIbkrDepthCopy(f->c, request, 12U, 100U, copy) == UMI_STATUS_OK);
        CHECK(copy->stale && !copy->received && copy->bidCount == 0U && copy->askCount == 0U);
        return 0;
    }
    if (!strcmp(mode, "cancel-late"))
        CHECK(UmiIbkrDepthCancel(f->c, request) == UMI_STATUS_OK);
    if (!strcmp(mode, "revision-exhausted"))
        f->c->depth[0].revision = UINT64_MAX;
    const char *price = !strcmp(mode, "negative-price") ? "-1.5"
                        : !strcmp(mode, "precision")    ? "0.1234567891"
                        : !strcmp(mode, "unset")        ? "1.7976931348623157e+308"
                        : !strcmp(mode, "scientific")   ? "1.25e2"
                                                        : "125";
    const char *size = !strcmp(mode, "negative-size") ? "-1" : "3.5";
    const char *position = !strcmp(mode, "gap") ? "1" : "0";
    const char *operation = !strcmp(mode, "update-empty") ? "1" : "0";
    const char *side = !strcmp(mode, "bad-side") ? "2" : "1";
    const char *maker = smart || !strcmp(mode, "maker") || !strcmp(mode, "wrong-smart") ? "EXAMPLE" : NULL;
    CHECK(Delta(f, !strcmp(mode, "foreign") ? "39999" : "36000", position, operation, side, price, size,
                maker, (smart || !strcmp(mode, "wrong-smart")) ? "1" : "0") == 0);
    UmiStatus status = UmiIbkrConnectionPump(f->c, 12U);
    bool invalid = !strcmp(mode, "precision") || !strcmp(mode, "unset") || !strcmp(mode, "negative-size") ||
                   !strcmp(mode, "gap") || !strcmp(mode, "update-empty") || !strcmp(mode, "bad-side") ||
                   !strcmp(mode, "wrong-smart") || !strcmp(mode, "revision-exhausted");
    if (invalid)
    {
        CHECK(status ==
              (!strcmp(mode, "revision-exhausted") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR));
        CHECK(UmiIbkrDepthCopy(f->c, request, 12U, 100U, copy) == UMI_STATUS_OK && copy->bidCount == 0U &&
              copy->stale);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiIbkrDepthCopy(f->c, request, 12U, 100U, copy) == UMI_STATUS_OK);
    if (!strcmp(mode, "foreign") || !strcmp(mode, "cancel-late"))
    {
        CHECK(copy->bidCount == 0U && copy->stale);
        return 0;
    }
    CHECK(copy->received && !copy->stale && copy->bidCount == 1U && copy->askCount == 0U);
    CHECK(copy->bids[0].size.coefficient == 35 && copy->bids[0].size.scale == 1U);
    if (!strcmp(mode, "negative-price"))
    {
        CHECK(copy->bids[0].price.coefficient == -15);
        return 0;
    }
    CHECK(copy->bids[0].price.coefficient == 125);
    if (maker != NULL)
    {
        CHECK(copy->bids[0].hasMarketMaker && !strcmp(copy->bids[0].marketMaker, "EXAMPLE"));
        return 0;
    }
    if (!strcmp(mode, "reset") || !strcmp(mode, "halt") || !strcmp(mode, "reset-exhausted"))
    {
        if (!strcmp(mode, "reset-exhausted"))
            f->c->depth[0].resetCount = UINT64_MAX;
        FEED(f, "4", "2", "36000", !strcmp(mode, "halt") ? "316" : "317", "Broker depth status");
        CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
        CHECK(UmiIbkrDepthCopy(f->c, request, 13U, 100U, copy) == UMI_STATUS_OK && copy->stale);
        if (strcmp(mode, "reset"))
        {
            CHECK(copy->failed && copy->bidCount == 1U);
            return 0;
        }
        CHECK(!copy->received && copy->bidCount == 0U && copy->askCount == 0U && copy->resetCount == 1U);
        CHECK(Delta(f, "36000", "0", "0", "0", "126", "2", NULL, "0") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 14U) == UMI_STATUS_OK);
        CHECK(UmiIbkrDepthCopy(f->c, request, 14U, 100U, copy) == UMI_STATUS_OK);
        CHECK(copy->bidCount == 0U && copy->askCount == 1U && !copy->stale);
        return 0;
    }
    if (!strcmp(mode, "disconnect"))
    {
        UmiIbkrConnectionClose(f->c);
        CHECK(UmiIbkrDepthCopy(f->c, request, 13U, 100U, copy) == UMI_STATUS_OK && copy->stale &&
              copy->bids[0].stale);
        return 0;
    }
    CHECK(Delta(f, "36000", "1", "0", "1", "124", "4", NULL, "0") == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 20U) == UMI_STATUS_OK);
    if (!strcmp(mode, "row-age"))
    {
        CHECK(UmiIbkrDepthCopy(f->c, request, 113U, 100U, copy) == UMI_STATUS_OK && !copy->stale);
        CHECK(copy->bids[0].stale && !copy->bids[1].stale);
        return 0;
    }
    if (!strcmp(mode, "full-insert"))
    {
        CHECK(Delta(f, "36000", "0", "0", "1", "126", "5", NULL, "0") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 21U) == UMI_STATUS_OK);
        CHECK(UmiIbkrDepthCopy(f->c, request, 21U, 100U, copy) == UMI_STATUS_OK && copy->bidCount == 2U);
        CHECK(copy->bids[0].price.coefficient == 126 && copy->bids[1].price.coefficient == 125);
        return 0;
    }
    if (!strcmp(mode, "update"))
    {
        CHECK(Delta(f, "36000", "1", "1", "1", "123", "8", NULL, "0") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 21U) == UMI_STATUS_OK);
        CHECK(UmiIbkrDepthCopy(f->c, request, 21U, 100U, copy) == UMI_STATUS_OK && copy->bidCount == 2U);
        CHECK(copy->bids[0].price.coefficient == 125 && copy->bids[1].price.coefficient == 123);
        return 0;
    }
    if (!strcmp(mode, "delete"))
    {
        CHECK(Delta(f, "36000", "0", "2", "1", "", "", NULL, "0") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 21U) == UMI_STATUS_OK);
        CHECK(UmiIbkrDepthCopy(f->c, request, 21U, 100U, copy) == UMI_STATUS_OK && copy->bidCount == 1U);
        CHECK(copy->bids[0].price.coefficient == 124 && copy->bids[1].price.coefficient == 0);
        return 0;
    }
    if (!strcmp(mode, "bad-delete"))
    {
        CHECK(Delta(f, "36000", "2", "2", "1", "0", "0", NULL, "0") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 21U) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiIbkrDepthCopy(f->c, request, 21U, 100U, copy) == UMI_STATUS_OK && copy->bidCount == 2U &&
              copy->stale);
    }
    return 0;
}
int main(int argc, char **argv)
{
    (void)PositionFeed;
    if (argc != 2)
        return 2;
    Fixture *f = New();
    UmiIbkrDepthSnapshot *copy = calloc(1U, sizeof *copy);
    if (f == NULL || copy == NULL)
    {
        Delete(f);
        free(copy);
        return 1;
    }
    int result = Run(f, argv[1], copy);
    Delete(f);
    free(copy);
    return result;
}
