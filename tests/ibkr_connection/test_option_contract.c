/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_option_contract.c
 * PURPOSE: Check selected-option framing, strict returned identity and ambiguous matches.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "discovery_fixture.h"
static int OptionDetail(Fixture *f, uint32_t request, const char *mode, const char *identity)
{
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"10",
                            id,
                            "WORKSHOP",
                            "OPT",
                            "20261218",
                            "100.0",
                            "C",
                            "SMART",
                            "USD",
                            "WORKSHOP CALL",
                            "Example market",
                            "CLASS",
                            identity,
                            "0.01",
                            "100",
                            "LMT,MKT",
                            "SMART",
                            "1",
                            "0",
                            "Workshop option",
                            "",
                            "",
                            "Industry",
                            "Category",
                            "Subcategory",
                            "UTC",
                            "",
                            "",
                            "",
                            "0",
                            "0",
                            "1",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "1",
                            "1",
                            "1"};
    if (!strcmp(mode, "symbol"))
        fields[2] = "OTHER";
    if (!strcmp(mode, "type"))
        fields[3] = "STK";
    if (!strcmp(mode, "expiry"))
        fields[4] = "20270115";
    if (!strcmp(mode, "strike"))
        fields[5] = "110";
    if (!strcmp(mode, "right"))
        fields[6] = "P";
    if (!strcmp(mode, "currency"))
        fields[8] = "GBP";
    if (!strcmp(mode, "class"))
        fields[11] = "OTHER";
    if (!strcmp(mode, "multiplier"))
        fields[14] = "10";
    if (!strcmp(mode, "put"))
        fields[6] = "P";
    if (!strcmp(mode, "future"))
        fields[3] = "FOP";
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    DiscoveryReferences();
    const char *mode = argv[1];
    static const char *const names[] = {
        "wire",        "put",          "future",       "pending-chain", "abandoned",
        "chain-index", "expiry-index", "strike-index", "invalid-right", "empty-currency",
        "precision",   "unsupported",  "queue-full",   "busy",          "complete",
        "empty",       "ambiguous",    "symbol",       "type",          "expiry",
        "strike",      "right",        "currency",     "class",         "multiplier"};
    if (!HistoricalKnownCase(mode, names, sizeof names / sizeof names[0]))
        return 2;
    Fixture *f = New();
    CHECK(f && Connect(f) == 0);
    UmiIbkrOptionChainQuery q = ChainQuery();
    if (!strcmp(mode, "future"))
        strcpy(q.underlyingSecurityType, "FUT");
    uint32_t chain, request = 999U;
    CHECK(UmiIbkrOptionChainRequest(f->c, &q, 10U, &chain) == UMI_STATUS_OK);
    CHECK(ChainFeed(f, chain, !strcmp(mode, "precision") ? "precision" : "complete") == 0);
    if (strcmp(mode, "pending-chain"))
        CHECK(ChainEnd(f, chain) == 0);
    CHECK(StreamPump(f, 11U) == 0);
    UmiIbkrOptionSelection select = {0};
    select.chainRequest = chain;
    select.right = 'C';
    strcpy(select.currency, "USD");
    strcpy(select.exchange, "SMART");
    if (!strcmp(mode, "put"))
        select.right = 'P';
    if (!strcmp(mode, "chain-index"))
        select.chainIndex = 99U;
    if (!strcmp(mode, "expiry-index"))
        select.expiryIndex = 99U;
    if (!strcmp(mode, "strike-index"))
        select.strikeIndex = 999U;
    if (!strcmp(mode, "invalid-right"))
        select.right = 'X';
    if (!strcmp(mode, "empty-currency"))
        select.currency[0] = 0;
    if (!strcmp(mode, "abandoned"))
        CHECK(UmiIbkrOptionChainAbandon(f->c, chain) == UMI_STATUS_OK);
    if (!strcmp(mode, "unsupported"))
        f->c->snapshot.protocolVersion = 163;
    if (!strcmp(mode, "queue-full"))
        f->c->txSize = UMI_IBKR_TX_LIMIT;
    if (!strcmp(mode, "busy"))
    {
        UmiIbkrQuoteContract contract = {123U, "SMART"};
        uint32_t old;
        CHECK(UmiIbkrContractDetailsRequest(f->c, &contract, 12U, &old) == UMI_STATUS_OK);
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(mode, "pending-chain") || !strcmp(mode, "abandoned"))
        expected = UMI_STATUS_INVALID_STATE;
    if (strstr(mode, "-index"))
        expected = UMI_STATUS_NOT_FOUND;
    if (!strcmp(mode, "invalid-right") || !strcmp(mode, "empty-currency"))
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (!strcmp(mode, "precision") || !strcmp(mode, "unsupported"))
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (!strcmp(mode, "queue-full"))
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (!strcmp(mode, "busy"))
        expected = UMI_STATUS_BUSY;
    size_t offset = f->outSize;
    CHECK(UmiIbkrOptionContractRequest(f->c, &select, 13U, &request) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(request == 999U);
    else
    {
        CHECK(StreamPump(f, 14U) == 0);
        if (!strcmp(mode, "wire") || !strcmp(mode, "put") || !strcmp(mode, "future"))
        {
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *fields[] = {"9",        "8",        id,
                                    "0",        "WORKSHOP", !strcmp(mode, "future") ? "FOP" : "OPT",
                                    "20261218", "100",      !strcmp(mode, "put") ? "P" : "C",
                                    "100",      "SMART",    "",
                                    "USD",      "",         "CLASS",
                                    "0",        "",         "",
                                    ""};
            CHECK(ObserveWire(f, offset, fields, 19U) == 0);
        }
        else
        {
            if (strcmp(mode, "empty"))
                CHECK(OptionDetail(f, request, mode, "789") == 0);
            if (!strcmp(mode, "ambiguous"))
                CHECK(OptionDetail(f, request, mode, "790") == 0);
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            FEED(f, "52", "1", id);
            bool valid = !strcmp(mode, "complete") || !strcmp(mode, "empty") || !strcmp(mode, "ambiguous");
            UmiStatus status = UmiIbkrConnectionPump(f->c, 15U);
            CHECK((status == UMI_STATUS_OK) == valid);
            UmiIbkrContractDetailsSnapshot *s = malloc(sizeof *s);
            CHECK(s);
            CHECK(UmiIbkrContractDetailsCopy(f->c, request, 15U, s) == UMI_STATUS_OK);
            if (valid)
            {
                CHECK(s->optionLookup && s->complete && !s->stale);
                CHECK(s->count == (!strcmp(mode, "empty") ? 0U : !strcmp(mode, "ambiguous") ? 2U : 1U));
            }
            else
                CHECK(!s->complete && s->stale && !s->count);
            free(s);
        }
    }
    Delete(f);
    return 0;
}
