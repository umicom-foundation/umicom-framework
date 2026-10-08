/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_pnl.c
 * PURPOSE: Check P&L wire identity, exact values, unavailable numbers and subscription ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_fixture.h"
#include <limits.h>
static int Run(Fixture *f, const char *mode)
{
    CHECK(Connect(f) == 0);
    UmiIbkrPnlSelection scope = {0};
    strcpy(scope.account, "DU123");
    uint32_t request = 99U;
    UmiIbkrPnlSnapshot copy;
    if (!strcmp(mode, "unlisted") || !strcmp(mode, "all-account"))
    {
        strcpy(scope.account, !strcmp(mode, "unlisted") ? "DU999" : "All");
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 10U, &request) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(request == 99U && f->c->pnl[0].requestId == 0U);
        return 0;
    }
    if (!strcmp(mode, "invalid-text") || !strcmp(mode, "unterminated") || !strcmp(mode, "contract-range"))
    {
        if (!strcmp(mode, "invalid-text"))
            strcpy(scope.modelCode, "bad\nmodel");
        if (!strcmp(mode, "unterminated"))
            memset(scope.account, 'a', sizeof scope.account);
        if (!strcmp(mode, "contract-range"))
            scope.contractId = UINT32_MAX;
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 10U, &request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(request == 99U);
        return 0;
    }
    if (!strcmp(mode, "unsupported") || !strcmp(mode, "not-ready") || !strcmp(mode, "queue-full") ||
        !strcmp(mode, "id-exhausted") || !strcmp(mode, "backward"))
    {
        UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (!strcmp(mode, "unsupported"))
        {
            f->c->snapshot.protocolVersion = 150;
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        }
        if (!strcmp(mode, "not-ready"))
        {
            f->c->snapshot.state = UMI_IBKR_WAITING;
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (!strcmp(mode, "queue-full"))
            f->c->txSize = UMI_IBKR_TX_LIMIT;
        if (!strcmp(mode, "id-exhausted"))
            f->c->nextQuoteRequest = INT_MAX;
        if (!strcmp(mode, "backward"))
            expected = UMI_STATUS_INVALID_ARGUMENT;
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, !strcmp(mode, "backward") ? 4U : 10U, &request) == expected);
        CHECK(request == 99U && f->c->pnl[0].requestId == 0U);
        return 0;
    }
    bool single =
        !strcmp(mode, "single") || !strcmp(mode, "single-wire") || !strcmp(mode, "single-cancel-wire");
    if (single)
    {
        scope.contractId = 123U;
        strcpy(scope.modelCode, "Growth");
    }
    size_t before = f->outSize;
    CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 10U, &request) == UMI_STATUS_OK && request == 36000U);
    if (!strcmp(mode, "wire") || !strcmp(mode, "single-wire"))
    {
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        const char *fields[] = {single ? "94" : "92", "36000", "DU123", single ? "Growth" : "", "123"};
        CHECK(ObserveWire(f, before, fields, single ? 5U : 4U) == 0);
        return 0;
    }
    if (!strcmp(mode, "duplicate"))
    {
        uint32_t other = 99U;
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 11U, &other) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(other == 99U);
        return 0;
    }
    if (!strcmp(mode, "different-scope"))
    {
        uint32_t other = 0U;
        strcpy(scope.account, "DU456");
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 11U, &other) == UMI_STATUS_OK && other == 36001U);
        strcpy(scope.modelCode, "Growth");
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 12U, &other) == UMI_STATUS_OK && other == 36002U);
        scope.contractId = 123U;
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 13U, &other) == UMI_STATUS_OK && other == 36003U);
        return 0;
    }
    if (!strcmp(mode, "capacity"))
    {
        for (uint32_t i = 1U; i < UMI_IBKR_PNL_CAPACITY; ++i)
        {
            scope.contractId = i;
            CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 10U, &request) == UMI_STATUS_OK);
        }
        scope.contractId = 100U;
        request = 99U;
        CHECK(UmiIbkrPnlSubscribe(f->c, &scope, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(request == 99U);
        return 0;
    }
    if (!strcmp(mode, "shared-identity"))
    {
        uint32_t search = 0U;
        CHECK(UmiIbkrSymbolSearchRequest(f->c, "Workshop", 11U, &search) == UMI_STATUS_OK &&
              search == 36001U);
        return 0;
    }
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    if (!strcmp(mode, "cancel-wire") || !strcmp(mode, "single-cancel-wire") || !strcmp(mode, "cancel-full"))
    {
        if (!strcmp(mode, "cancel-full"))
        {
            f->c->txSize = UMI_IBKR_TX_LIMIT;
            CHECK(UmiIbkrPnlCancel(f->c, request) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(UmiIbkrPnlCopy(f->c, request, 11U, 100U, &copy) == UMI_STATUS_OK && copy.subscribed);
            return 0;
        }
        before = f->outSize;
        CHECK(UmiIbkrPnlCancel(f->c, request) == UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
        const char *fields[] = {single ? "95" : "93", "36000"};
        CHECK(ObserveWire(f, before, fields, 2U) == 0);
        CHECK(UmiIbkrPnlCopy(f->c, request, 12U, 100U, &copy) == UMI_STATUS_OK && !copy.subscribed &&
              copy.stale);
        size_t pending = f->c->txSize;
        CHECK(UmiIbkrPnlCancel(f->c, request) == UMI_STATUS_OK && f->c->txSize == pending);
        return 0;
    }
    if (!strcmp(mode, "alias-reuse"))
    {
        CHECK(UmiIbkrPnlCancel(f->c, request) == UMI_STATUS_OK);
        CHECK(UmiIbkrPnlSubscribe(f->c, &f->c->pnl[0].selection, 12U, &request) == UMI_STATUS_OK &&
              request == 36001U);
        CHECK(!strcmp(f->c->pnl[0].selection.account, "DU123"));
        return 0;
    }
    if (!strcmp(mode, "waiting"))
    {
        CHECK(UmiIbkrPnlCopy(f->c, request, 12U, 100U, &copy) == UMI_STATUS_OK);
        CHECK(copy.stale && !copy.received && !copy.daily.exact);
        return 0;
    }
    if (!strcmp(mode, "refusal"))
    {
        FEED(f, "4", "2", "36000", "200", "Private details must not enter an observation message");
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrPnlCopy(f->c, request, 12U, 100U, &copy) == UMI_STATUS_OK);
        CHECK(copy.failed && copy.stale && copy.providerCode == 200);
        CHECK(strstr(copy.message, "Private") == NULL && f->c->snapshot.state == UMI_IBKR_READY);
        return 0;
    }
    if (!strcmp(mode, "cancel-late"))
        CHECK(UmiIbkrPnlCancel(f->c, request) == UMI_STATUS_OK);
    if (!strcmp(mode, "revision-exhausted"))
        f->c->pnl[0].revision = UINT64_MAX;
    const char *daily = "12.5", *realized = "0", *id = !strcmp(mode, "foreign") ? "39999" : "36000";
    if (!strcmp(mode, "unset"))
        daily = "1.7976931348623157e+308";
    if (!strcmp(mode, "empty"))
        daily = "";
    if (!strcmp(mode, "precision"))
        daily = "0.1234567891";
    if (!strcmp(mode, "scientific"))
        daily = "125e-1";
    if (!strcmp(mode, "malformed"))
        realized = "not-a-number";
    if (!strcmp(mode, "nan"))
        realized = "NaN";
    if (!strcmp(mode, "invalid-utf8"))
        realized = "\xc0\xaf";
    if (single)
    {
        FEED(f, "95", id, "-3", daily, "-2.75", realized, "123.5");
    }
    else if (!strcmp(mode, "wrong-kind"))
    {
        FEED(f, "95", id, "3", daily, "-2.75", realized, "123.5");
    }
    else if (!strcmp(mode, "missing-field"))
    {
        FEED(f, "94", id, daily, "-2.75");
    }
    else
    {
        FEED(f, "94", id, daily, "-2.75", realized);
    }
    UmiStatus status = UmiIbkrConnectionPump(f->c, 12U);
    bool bad = !strcmp(mode, "malformed") || !strcmp(mode, "nan") || !strcmp(mode, "invalid-utf8") ||
               !strcmp(mode, "wrong-kind") || !strcmp(mode, "missing-field") ||
               !strcmp(mode, "revision-exhausted");
    if (bad)
    {
        CHECK(status ==
              (!strcmp(mode, "revision-exhausted") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR));
        CHECK(UmiIbkrPnlCopy(f->c, request, 12U, 100U, &copy) == UMI_STATUS_OK);
        CHECK(!copy.received && copy.daily.reportedText[0] == '\0');
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiIbkrPnlCopy(f->c, request, 12U, 100U, &copy) == UMI_STATUS_OK);
    if (!strcmp(mode, "foreign") || !strcmp(mode, "cancel-late"))
    {
        CHECK(!copy.received && copy.stale);
        return 0;
    }
    CHECK(copy.received && !copy.stale && copy.revision == 2U && copy.realized.exact);
    CHECK(copy.realized.value.coefficient == 0 && copy.unrealized.exact);
    if (!strcmp(mode, "unset") || !strcmp(mode, "empty") || !strcmp(mode, "precision"))
    {
        CHECK(!copy.daily.exact && !strcmp(copy.daily.reportedText, daily));
        return 0;
    }
    CHECK(copy.daily.exact && copy.daily.value.coefficient == 125 && copy.daily.value.scale == 1U);
    if (single)
        CHECK(copy.position.exact && copy.position.value.coefficient == -3 && copy.marketValue.exact);
    if (!strcmp(mode, "age"))
    {
        CHECK(UmiIbkrPnlCopy(f->c, request, 112U, 100U, &copy) == UMI_STATUS_OK && !copy.stale);
        CHECK(UmiIbkrPnlCopy(f->c, request, 113U, 100U, &copy) == UMI_STATUS_OK && copy.stale);
    }
    if (!strcmp(mode, "disconnect"))
    {
        UmiIbkrConnectionClose(f->c);
        CHECK(UmiIbkrPnlCopy(f->c, request, 12U, 100U, &copy) == UMI_STATUS_OK && copy.stale);
        CHECK(UmiIbkrPnlCancel(f->c, request) == UMI_STATUS_INVALID_STATE);
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
    int status = Run(f, argv[1]);
    Delete(f);
    return status;
}
