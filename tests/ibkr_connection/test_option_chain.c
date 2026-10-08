/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_option_chain.c
 * PURPOSE: Check option discovery request ownership, completion, timeout and malformed record rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "discovery_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    DiscoveryReferences();
    const char *mode = argv[1];
    static const char *const names[] = {"wire",
                                        "not-ready",
                                        "invalid-contract",
                                        "invalid-symbol",
                                        "invalid-exchange",
                                        "unsupported",
                                        "backward",
                                        "queue-full",
                                        "overflow",
                                        "busy",
                                        "abandon",
                                        "late-abandon",
                                        "timeout",
                                        "late-timeout",
                                        "empty",
                                        "disconnect",
                                        "provider-error",
                                        "partial",
                                        "complete",
                                        "underlying",
                                        "exchange",
                                        "class",
                                        "utf8",
                                        "expiries",
                                        "date",
                                        "duplicate-expiry",
                                        "strikes",
                                        "number",
                                        "precision",
                                        "negative",
                                        "duplicate-strike",
                                        "short",
                                        "extra",
                                        "duplicate-chain",
                                        "foreign",
                                        "fragmented",
                                        "reuse"};
    if (!HistoricalKnownCase(mode, names, sizeof names / sizeof names[0]))
        return 2;
    Fixture *f = New();
    CHECK(f);
    UmiIbkrOptionChainQuery q = ChainQuery();
    uint32_t request = 999U;
    if (!strcmp(mode, "not-ready"))
    {
        CHECK(UmiIbkrOptionChainRequest(f->c, &q, 0U, &request) == UMI_STATUS_INVALID_STATE &&
              request == 999U);
        Delete(f);
        return 0;
    }
    CHECK(Connect(f) == 0);
    if (!strcmp(mode, "invalid-contract"))
        q.underlyingContractId = 0U;
    if (!strcmp(mode, "invalid-symbol"))
        q.underlyingSymbol[0] = 0;
    if (!strcmp(mode, "invalid-exchange"))
        memset(q.exchange, 'x', sizeof q.exchange);
    if (!strncmp(mode, "invalid-", 8U) || !strcmp(mode, "backward"))
    {
        CHECK(UmiIbkrOptionChainRequest(f->c, &q, !strcmp(mode, "backward") ? 4U : 10U, &request) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              request == 999U);
    }
    else if (!strcmp(mode, "unsupported"))
    {
        f->c->snapshot.protocolVersion = 177;
        CHECK(UmiIbkrOptionChainRequest(f->c, &q, 10U, &request) == UMI_STATUS_NOT_IMPLEMENTED);
    }
    else if (!strcmp(mode, "queue-full"))
    {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrOptionChainRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->optionChains && request == 999U && !f->c->optionChainRequested);
    }
    else if (!strcmp(mode, "overflow"))
    {
        f->c->nextQuoteRequest = INT32_MAX;
        CHECK(UmiIbkrOptionChainRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else
    {
        size_t offset = f->outSize;
        CHECK(UmiIbkrOptionChainRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK && StreamPump(f, 11U) == 0);
        UmiIbkrOptionChainSnapshot s;
        CHECK(UmiIbkrOptionChainCopy(f->c, request, 11U, &s) == UMI_STATUS_OK && !s.complete && s.stale);
        if (!strcmp(mode, "wire"))
        {
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *expected[] = {"78", id, "WORKSHOP", "", "STK", "123"};
            CHECK(ObserveWire(f, offset, expected, 6U) == 0);
        }
        else if (!strcmp(mode, "busy"))
        {
            uint32_t other = 999U;
            CHECK(UmiIbkrOptionChainRequest(f->c, &q, 1010U, &other) == UMI_STATUS_BUSY && other == 999U);
        }
        else if (!strcmp(mode, "abandon") || !strcmp(mode, "late-abandon") || !strcmp(mode, "reuse"))
        {
            CHECK(UmiIbkrOptionChainAbandon(f->c, request) == UMI_STATUS_OK);
            CHECK(ChainFeed(f, request, "complete") == 0 && ChainEnd(f, request) == 0 &&
                  StreamPump(f, 12U) == 0);
            CHECK(UmiIbkrOptionChainCopy(f->c, request, 12U, &s) == UMI_STATUS_OK && s.abandoned &&
                  !s.complete && !s.count);
            if (!strcmp(mode, "reuse"))
            {
                uint32_t previous = request;
                CHECK(UmiIbkrOptionChainRequest(f->c, &q, 1010U, &request) == UMI_STATUS_OK &&
                      request > previous);
                CHECK(UmiIbkrOptionChainCopy(f->c, previous, 1010U, &s) == UMI_STATUS_NOT_FOUND);
            }
        }
        else if (!strcmp(mode, "timeout") || !strcmp(mode, "late-timeout"))
        {
            CHECK(UmiIbkrOptionChainCopy(f->c, request, 60010U, &s) == UMI_STATUS_OK && s.failed && s.stale);
            if (!strcmp(mode, "late-timeout"))
            {
                CHECK(ChainFeed(f, request, "complete") == 0 && ChainEnd(f, request) == 0 &&
                      StreamPump(f, 60010U) == 0);
                CHECK(UmiIbkrOptionChainCopy(f->c, request, 60010U, &s) == UMI_STATUS_OK && !s.count &&
                      !s.complete);
            }
        }
        else if (!strcmp(mode, "disconnect"))
        {
            UmiIbkrConnectionClose(f->c);
            CHECK(UmiIbkrOptionChainCopy(f->c, request, 12U, &s) == UMI_STATUS_OK && s.failed && s.stale);
        }
        else if (!strcmp(mode, "provider-error"))
        {
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            FEED(f, "4", "2", id, "200", "Unknown underlying");
            CHECK(StreamPump(f, 12U) == 0 &&
                  UmiIbkrOptionChainCopy(f->c, request, 12U, &s) == UMI_STATUS_OK && s.failed &&
                  s.providerCode == 200);
            CHECK(f->c->snapshot.state == UMI_IBKR_READY);
        }
        else
        {
            if (!strcmp(mode, "fragmented"))
                f->readStep = 1U;
            if (!strcmp(mode, "duplicate-chain"))
                CHECK(ChainFeed(f, request, "complete") == 0 && StreamPump(f, 12U) == 0);
            if (strcmp(mode, "empty"))
                CHECK(ChainFeed(f, !strcmp(mode, "foreign") ? request + 99U : request, mode) == 0);
            bool accepted = !strcmp(mode, "partial") || !strcmp(mode, "complete") || !strcmp(mode, "empty") ||
                            !strcmp(mode, "precision") || !strcmp(mode, "negative") ||
                            !strcmp(mode, "foreign") || !strcmp(mode, "fragmented");
            if (strcmp(mode, "partial"))
                CHECK(ChainEnd(f, request) == 0);
            UmiStatus status = UmiIbkrConnectionPump(f->c, 13U);
            CHECK((status == UMI_STATUS_OK) == accepted);
            CHECK(UmiIbkrOptionChainCopy(f->c, request, 13U, &s) == UMI_STATUS_OK);
            if (!accepted)
                CHECK(s.failed && !s.complete && s.count == (!strcmp(mode, "duplicate-chain") ? 1U : 0U));
            else
            {
                CHECK(s.complete == (strcmp(mode, "partial") != 0));
                CHECK(s.count == (!strcmp(mode, "empty") || !strcmp(mode, "foreign") ? 0U : 1U));
                if (s.count)
                {
                    UmiIbkrOptionChain *row = malloc(sizeof *row);
                    CHECK(row);
                    CHECK(UmiIbkrOptionChainItemCopy(f->c, request, 0U, row) == UMI_STATUS_OK &&
                          row->expiryCount == 2U && row->strikeCount == 2U);
                    CHECK(row->strikes[0].exact == (strcmp(mode, "precision") != 0));
                    free(row);
                }
            }
        }
    }
    Delete(f);
    return 0;
}
