/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_symbol_search.c
 * PURPOSE: Check atomic discovery results, protocol layouts and paced request replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_fixture.h"
#include <limits.h>
static int Candidate(Fixture *f, const char *id, const char *mode)
{
    const char *fields[] = {"79",  id,  "1",   "123", "WORKSHOP",     "STK",   "LSE",
                            "GBP", "2", "OPT", "FUT", "Workshop plc", "issuer"};
    if (!strcmp(mode, "bad-id"))
        fields[3] = "0";
    if (!strcmp(mode, "bad-count"))
        fields[2] = "17";
    if (!strcmp(mode, "bad-derivatives"))
        fields[8] = "17";
    if (!strcmp(mode, "invalid-utf8"))
        fields[11] = "\xc0\xaf";
    if (!strcmp(mode, "empty-symbol"))
        fields[4] = "";
    if (!strcmp(mode, "long-description"))
    {
        static char description[300];
        memset(description, 'x', sizeof description - 1U);
        fields[11] = description;
    }
    size_t count = sizeof fields / sizeof fields[0];
    if (!strcmp(mode, "legacy"))
        count = 11U;
    if (!strcmp(mode, "truncated"))
        --count;
    return Feed(f, fields, count);
}
static int Run(Fixture *f, const char *mode, UmiIbkrSymbolSearchSnapshot *copy)
{
    CHECK(Connect(f) == 0);
    uint32_t request = 99U;
    if (!strcmp(mode, "empty") || !strcmp(mode, "spaces") || !strcmp(mode, "control") ||
        !strcmp(mode, "invalid-pattern") || !strcmp(mode, "long-pattern"))
    {
        char longPattern[140];
        memset(longPattern, 'x', sizeof longPattern - 1U);
        longPattern[sizeof longPattern - 1U] = '\0';
        const char *pattern = !strcmp(mode, "empty")             ? ""
                              : !strcmp(mode, "spaces")          ? "   "
                              : !strcmp(mode, "control")         ? "abc\n"
                              : !strcmp(mode, "invalid-pattern") ? "\xc0\xaf"
                                                                 : longPattern;
        CHECK(UmiIbkrSymbolSearchRequest(f->c, pattern, 10U, &request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(request == 99U && f->c->symbolSearch.requestId == 0U);
        return 0;
    }
    if (!strcmp(mode, "queue-full") || !strcmp(mode, "unsupported") || !strcmp(mode, "backward") ||
        !strcmp(mode, "id-exhausted") || !strcmp(mode, "not-ready"))
    {
        UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (!strcmp(mode, "queue-full"))
            f->c->txSize = UMI_IBKR_TX_LIMIT;
        if (!strcmp(mode, "unsupported"))
        {
            f->c->snapshot.protocolVersion = 177;
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        }
        if (!strcmp(mode, "id-exhausted"))
            f->c->nextQuoteRequest = INT_MAX;
        if (!strcmp(mode, "not-ready"))
        {
            f->c->snapshot.state = UMI_IBKR_WAITING;
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (!strcmp(mode, "backward"))
            expected = UMI_STATUS_INVALID_ARGUMENT;
        CHECK(UmiIbkrSymbolSearchRequest(f->c, "Workshop", !strcmp(mode, "backward") ? 4U : 10U, &request) ==
              expected);
        CHECK(request == 99U && f->c->symbolSearch.requestId == 0U);
        return 0;
    }
    if (!strcmp(mode, "legacy"))
        f->c->snapshot.protocolVersion = 175;
    size_t before = f->outSize;
    CHECK(UmiIbkrSymbolSearchRequest(f->c, "Workshop", 10U, &request) == UMI_STATUS_OK && request == 36000U);
    if (!strcmp(mode, "wire"))
    {
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        const char *fields[] = {"81", "36000", "Workshop"};
        CHECK(ObserveWire(f, before, fields, 3U) == 0);
        return 0;
    }
    if (!strcmp(mode, "pending") || !strcmp(mode, "paced"))
    {
        if (!strcmp(mode, "paced"))
            CHECK(UmiIbkrSymbolSearchAbandon(f->c, request) == UMI_STATUS_OK);
        uint32_t other = 99U;
        CHECK(UmiIbkrSymbolSearchRequest(f->c, "Other", 11U, &other) == UMI_STATUS_BUSY && other == 99U);
        if (!strcmp(mode, "paced"))
            CHECK(UmiIbkrSymbolSearchRequest(f->c, "Other", 1010U, &other) == UMI_STATUS_OK &&
                  other == 36001U);
        return 0;
    }
    if (!strcmp(mode, "timeout"))
    {
        CHECK(UmiIbkrSymbolSearchCopy(f->c, request, 10010U, copy) == UMI_STATUS_OK && copy->failed &&
              copy->stale);
        CHECK(UmiIbkrSymbolSearchRequest(f->c, "Other", 10010U, &request) == UMI_STATUS_OK &&
              request == 36001U);
        return 0;
    }
    if (!strcmp(mode, "alias-pattern"))
    {
        CHECK(UmiIbkrSymbolSearchAbandon(f->c, request) == UMI_STATUS_OK);
        CHECK(UmiIbkrSymbolSearchRequest(f->c, f->c->symbolSearch.pattern, 1010U, &request) == UMI_STATUS_OK);
        CHECK(!strcmp(f->c->symbolSearch.pattern, "Workshop"));
        return 0;
    }
    if (!strcmp(mode, "refusal"))
    {
        FEED(f, "4", "2", "36000", "200", "Private broker message");
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrSymbolSearchCopy(f->c, request, 11U, copy) == UMI_STATUS_OK && copy->failed);
        CHECK(copy->providerCode == 200 && strstr(copy->message, "Private") == NULL);
        CHECK(f->c->snapshot.state == UMI_IBKR_READY);
        return 0;
    }
    if (!strcmp(mode, "abandon") || !strcmp(mode, "late-request") || !strcmp(mode, "late-error"))
    {
        CHECK(UmiIbkrSymbolSearchAbandon(f->c, request) == UMI_STATUS_OK);
        if (strcmp(mode, "abandon"))
            CHECK(UmiIbkrSymbolSearchRequest(f->c, "Other", 1010U, &request) == UMI_STATUS_OK);
        if (!strcmp(mode, "late-error"))
        {
            FEED(f, "4", "2", "36000", "200", "Late old response");
        }
        else
            CHECK(Candidate(f, "36000", "valid") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 1011U) == UMI_STATUS_OK);
        CHECK(UmiIbkrSymbolSearchCopy(f->c, request, 1011U, copy) == UMI_STATUS_OK && !copy->complete &&
              copy->count == 0U);
        CHECK(copy->failed == (!strcmp(mode, "abandon")));
        return 0;
    }
    if (!strcmp(mode, "no-results"))
    {
        FEED(f, "79", "36000", "0");
    }
    else
        CHECK(Candidate(f, !strcmp(mode, "foreign") ? "39999" : "36000", mode) == 0);
    UmiStatus status = UmiIbkrConnectionPump(f->c, 11U);
    bool invalid = !strcmp(mode, "bad-id") || !strcmp(mode, "bad-count") ||
                   !strcmp(mode, "bad-derivatives") || !strcmp(mode, "invalid-utf8") ||
                   !strcmp(mode, "empty-symbol") || !strcmp(mode, "long-description") ||
                   !strcmp(mode, "truncated");
    if (invalid)
    {
        CHECK(status == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiIbkrSymbolSearchCopy(f->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(copy->count == 0U && !copy->complete && copy->stale);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiIbkrSymbolSearchCopy(f->c, request, 11U, copy) == UMI_STATUS_OK);
    if (!strcmp(mode, "foreign"))
    {
        CHECK(!copy->complete && copy->count == 0U);
        return 0;
    }
    CHECK(copy->complete && !copy->stale && !copy->failed);
    if (!strcmp(mode, "no-results"))
    {
        CHECK(copy->count == 0U);
        return 0;
    }
    CHECK(copy->count == 1U && copy->rows[0].contractId == 123U && copy->rows[0].derivativeTypeCount == 2U);
    CHECK(!strcmp(copy->rows[0].primaryExchange, "LSE") && !strcmp(copy->rows[0].derivativeTypes[1], "FUT"));
    CHECK(!strcmp(copy->rows[0].description, !strcmp(mode, "legacy") ? "" : "Workshop plc"));
    if (!strcmp(mode, "duplicate"))
    {
        CHECK(Candidate(f, "36000", "bad-id") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrSymbolSearchCopy(f->c, request, 12U, copy) == UMI_STATUS_OK && copy->count == 1U);
    }
    if (!strcmp(mode, "disconnect"))
    {
        UmiIbkrConnectionClose(f->c);
        CHECK(UmiIbkrSymbolSearchCopy(f->c, request, 12U, copy) == UMI_STATUS_OK && copy->stale);
    }
    return 0;
}
int main(int argc, char **argv)
{
    (void)PositionFeed;
    if (argc != 2)
        return 2;
    Fixture *f = New();
    UmiIbkrSymbolSearchSnapshot *copy = calloc(1U, sizeof *copy);
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
