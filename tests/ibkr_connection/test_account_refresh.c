/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_account_refresh.c
 *
 * PURPOSE:
 *   Verify repeat account capture identities, late replies and transactional admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/broker_connectivity/account_query.h"
#include <limits.h>

static int Refresh(Fixture *f, const char *account, uint64_t now)
{
    CHECK(UmiIbkrConnectionRefreshAccountReport(f->c, account,
        UMI_IBKR_ACCOUNT_OVERVIEW, "", now) == UMI_STATUS_OK);
    return 0;
}
static void Id(uint32_t value, char out[24])
{
    (void)snprintf(out, 24U, "%u", (unsigned)value);
}
static int Value(Fixture *f, uint32_t request, const char *account, const char *value)
{
    char id[24]; Id(request, id);
    FEED(f, "63", "1", id, account, "NetLiquidation", value, "USD");
    return 0;
}
static int Position(Fixture *f, uint32_t request, const char *account,
    const char *quantity, const char *model)
{
    char id[24]; Id(request, id);
    FEED(f, "71", "1", id, account, "123", "SAMPLE", "STK", "", "0", "", "",
        "SMART", "USD", "SAMPLE", "SAMPLE", quantity, "100.25", model);
    return 0;
}
static int End(Fixture *f, bool summary, uint32_t request)
{
    char id[24]; Id(request, id);
    FEED(f, summary ? "64" : "72", "1", id);
    return 0;
}
static int Complete(Fixture *f, uint64_t now)
{
    CHECK(End(f, true, f->c->accountSummaryRequest) == 0);
    CHECK(End(f, false, f->c->accountPositionsRequest) == 0);
    CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
    CHECK(f->c->snapshot.summaryComplete && f->c->snapshot.positionsComplete);
    return 0;
}
/* Inspect complete outgoing frames rather than searching substrings. A request
 * and its cancellation must retain their own identity and selected account. */
static int Frame(const unsigned char *bytes, size_t length, const char *const *fields, size_t count)
{
    size_t body = 0U;
    for (size_t i = 0; i < count; ++i) body += strlen(fields[i]) + 1U;
    CHECK(length >= body + 4U);
    size_t encoded = ((size_t)bytes[0] << 24U) | ((size_t)bytes[1] << 16U) |
        ((size_t)bytes[2] << 8U) | bytes[3];
    CHECK(encoded == body);
    size_t at = 4U;
    for (size_t i = 0; i < count; ++i) {
        size_t n = strlen(fields[i]) + 1U;
        CHECK(!memcmp(bytes + at, fields[i], n)); at += n;
    }
    return (int)at;
}
static int Run(Fixture *f, const char *name)
{
    UmiIbkrConnection *c = f->c;
    if (!strcmp(name, "before-ready")) {
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, "DU123", UMI_IBKR_ACCOUNT_OVERVIEW,
            "", 0U) == UMI_STATUS_INVALID_STATE);
        return 0;
    }
    CHECK(Connect(f) == 0);
    if (!strcmp(name, "null")) {
        CHECK(UmiIbkrConnectionRefreshAccountReport(NULL, "DU123", UMI_IBKR_ACCOUNT_OVERVIEW, "", 10U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, NULL, UMI_IBKR_ACCOUNT_OVERVIEW, "", 10U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, "DU123", UMI_IBKR_ACCOUNT_OVERVIEW, NULL, 10U) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (!strcmp(name, "invalid") || !strcmp(name, "unlisted") || !strcmp(name, "backwards") ||
        !strcmp(name, "stale") || !strcmp(name, "version") || !strcmp(name, "exhausted")) {
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        const char *account = "DU123", *currency = ""; uint64_t now = 10U;
        UmiIbkrAccountReportKind kind = UMI_IBKR_ACCOUNT_OVERVIEW;
        if (!strcmp(name, "invalid")) { kind = UMI_IBKR_ACCOUNT_CURRENCY; currency = "ALL"; }
        if (!strcmp(name, "unlisted")) { account = "DU999"; expected = UMI_STATUS_PERMISSION_DENIED; }
        if (!strcmp(name, "backwards")) now = 4U;
        if (!strcmp(name, "stale")) { c->snapshot.stale = true; expected = UMI_STATUS_INVALID_STATE; }
        if (!strcmp(name, "version")) { c->snapshot.protocolVersion = 177; expected = UMI_STATUS_UNAVAILABLE; }
        if (!strcmp(name, "exhausted")) { c->nextQuoteRequest = (uint32_t)INT_MAX - 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        UmiIbkrConnectionSnapshot before = c->snapshot; size_t queued = c->txSize;
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, account, kind, currency, now) == expected);
        CHECK(!memcmp(&before, &c->snapshot, sizeof before) && c->txSize == queued);
        return 0;
    }
    if (!strcmp(name, "queue-first") || !strcmp(name, "queue-second")) {
        char tags[768]; CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_OVERVIEW, "", tags, sizeof tags) == UMI_STATUS_OK);
        /* The summary body is 16 + strlen(tags) bytes; retain room for that
         * frame alone to force the second queue operation to fail. */
        c->txSize = UMI_IBKR_TX_LIMIT - (!strcmp(name, "queue-first") ? 1U : strlen(tags) + 20U);
        size_t queued = c->txSize; uint32_t sequence = c->nextQuoteRequest;
        UmiIbkrConnectionSnapshot before = c->snapshot;
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, "DU123", UMI_IBKR_ACCOUNT_OVERVIEW,
            "", 10U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(c->txSize == queued && c->nextQuoteRequest == sequence);
        CHECK(!memcmp(&before, &c->snapshot, sizeof before));
        return 0;
    }
    if (!strcmp(name, "legacy-transition")) {
        CHECK(UmiIbkrConnectionReadAccount(c, "DU123", 10U) == UMI_STATUS_OK);
        FEED(f, "64", "1", "35001"); FEED(f, "62", "1");
        CHECK(UmiIbkrConnectionPump(c, 11U) == UMI_STATUS_OK);
        CHECK(Refresh(f, "DU456", 12U) == 0);
        FEED(f, "63", "1", "35001", "DU456", "NetLiquidation", "999", "USD");
        CHECK(PositionFeed(f, "DU456", "99", "1") == 0); FEED(f, "62", "1");
        CHECK(UmiIbkrConnectionPump(c, 13U) == UMI_STATUS_OK);
        CHECK(!c->snapshot.valueCount && !c->snapshot.positionCount && !c->snapshot.positionsComplete);
        return 0;
    }
    CHECK(Refresh(f, "DU123", 10U) == 0);
    const uint32_t summary = c->accountSummaryRequest, positions = c->accountPositionsRequest;
    CHECK(summary == 36000U && positions == 36001U && c->nextQuoteRequest == 36002U);
    if (!strcmp(name, "wire")) {
        const char *a[] = {"62", "1", "36000", "All", "NetLiquidation,TotalCashValue,BuyingPower,AvailableFunds"};
        const char *b[] = {"74", "1", "36001", "DU123", ""};
        int first = Frame(c->tx, c->txSize, a, 5U); CHECK(first > 1);
        int second = Frame(c->tx + first, c->txSize - (size_t)first, b, 5U); CHECK(second > 1);
        CHECK((size_t)(first + second) == c->txSize);
        return 0;
    }
    if (!strcmp(name, "pending") || !strcmp(name, "legacy-after")) {
        UmiIbkrConnectionSnapshot before = c->snapshot; size_t queued = c->txSize;
        if (!strcmp(name, "pending"))
            CHECK(UmiIbkrConnectionRefreshAccountReport(c, "DU456", UMI_IBKR_ACCOUNT_MARGIN, "", 11U) == UMI_STATUS_BUSY);
        else CHECK(UmiIbkrConnectionReadAccount(c, "DU456", 11U) == UMI_STATUS_INVALID_STATE);
        CHECK(!memcmp(&before, &c->snapshot, sizeof before) && c->txSize == queued);
        return 0;
    }
    if (!strcmp(name, "timeout")) {
        CHECK(UmiIbkrConnectionPump(c, 10010U) == UMI_STATUS_TIMEOUT);
        CHECK(c->snapshot.stale && !c->snapshot.summaryComplete);
        return 0;
    }
    if (!strcmp(name, "disconnect")) {
        UmiIbkrConnectionClose(c);
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, "DU123", UMI_IBKR_ACCOUNT_OVERVIEW,
            "", 11U) == UMI_STATUS_INVALID_STATE);
        return 0;
    }
    if (!strcmp(name, "malformed") || !strcmp(name, "field-count") || !strcmp(name, "bad-version")) {
        if (!strcmp(name, "malformed")) CHECK(Position(f, positions, "DU123", "not-a-number", "") == 0);
        if (!strcmp(name, "field-count")) FEED(f, "71", "1", "36001", "DU123");
        if (!strcmp(name, "bad-version")) FEED(f, "72", "2", "36001");
        CHECK(UmiIbkrConnectionPump(c, 11U) == UMI_STATUS_PARSE_ERROR);
        CHECK(c->snapshot.stale && !c->snapshot.positionsComplete);
        return 0;
    }
    if (!strcmp(name, "active-summary-error") || !strcmp(name, "active-position-error")) {
        FEED(f, "4", "2", !strcmp(name, "active-summary-error") ? "36000" : "36001", "321", "Synthetic request refused");
        CHECK(UmiIbkrConnectionPump(c, 11U) == UMI_STATUS_UNAVAILABLE);
        CHECK(c->snapshot.providerCode == 321 && c->snapshot.stale);
        return 0;
    }
    if (!strcmp(name, "independent")) {
        CHECK(End(f, true, summary) == 0);
        CHECK(UmiIbkrConnectionPump(c, 11U) == UMI_STATUS_OK);
        CHECK(c->snapshot.summaryComplete && !c->snapshot.positionsComplete);
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, "DU456", UMI_IBKR_ACCOUNT_OVERVIEW, "", 12U) == UMI_STATUS_BUSY);
        CHECK(End(f, false, positions) == 0);
        CHECK(UmiIbkrConnectionPump(c, 13U) == UMI_STATUS_OK);
        CHECK(Refresh(f, "DU456", 14U) == 0);
        return 0;
    }
    if (!strcmp(name, "wrong-account") || !strcmp(name, "wrong-request") || !strcmp(name, "model")) {
        CHECK(Value(f, !strcmp(name, "wrong-request") ? summary + 50U : summary,
            !strcmp(name, "wrong-account") ? "DU456" : "DU123", "123.45") == 0);
        CHECK(Position(f, !strcmp(name, "wrong-request") ? positions + 50U : positions,
            !strcmp(name, "wrong-account") ? "DU456" : "DU123", "2.5", !strcmp(name, "model") ? "MODEL" : "") == 0);
        CHECK(UmiIbkrConnectionPump(c, 11U) == UMI_STATUS_OK);
        CHECK(c->snapshot.positionCount == 0U);
        CHECK(c->snapshot.valueCount == (!strcmp(name, "model") ? 1U : 0U));
        return 0;
    }
    if (!strcmp(name, "wrong-end")) {
        CHECK(End(f, true, summary + 50U) == 0); CHECK(End(f, false, positions + 50U) == 0);
        FEED(f, "62", "1");
        CHECK(UmiIbkrConnectionPump(c, 11U) == UMI_STATUS_OK);
        CHECK(!c->snapshot.summaryComplete && !c->snapshot.positionsComplete);
        return 0;
    }
    if (!strcmp(name, "fragmented")) f->readStep = 1U;
    if (strcmp(name, "empty") != 0) {
        CHECK(Value(f, summary, "DU123", "123456789.123456789") == 0);
        CHECK(Position(f, positions, "DU123", "2.5", "") == 0);
        CHECK(Position(f, positions, "DU123", "3.5", "") == 0);
    }
    CHECK(Complete(f, 11U) == 0);
    if (!strcmp(name, "empty")) { CHECK(!c->snapshot.valueCount && !c->snapshot.positionCount); return 0; }
    CHECK(c->snapshot.valueCount == 1U && c->snapshot.positionCount == 1U);
    CHECK(!strcmp(c->snapshot.values[0].value, "123456789.123456789"));
    CHECK(!strcmp(c->snapshot.positions[0].quantity, "3.5"));
    if (!strcmp(name, "values") || !strcmp(name, "fragmented")) return 0;
    if (!strcmp(name, "cancellation")) {
        const char *a[] = {"63", "1", "36000"}, *b[] = {"75", "1", "36001"};
        int first = Frame(c->tx, c->txSize, a, 3U); CHECK(first > 1);
        int second = Frame(c->tx + first, c->txSize - (size_t)first, b, 3U); CHECK(second > 1);
        CHECK((size_t)(first + second) == c->txSize); return 0;
    }
    if (!strcmp(name, "completed-late")) {
        CHECK(Value(f, summary, "DU123", "9") == 0);
        CHECK(Position(f, positions, "DU123", "99", "") == 0);
        CHECK(End(f, false, positions) == 0);
        FEED(f, "4", "2", "36001", "321", "Retired request");
        CHECK(UmiIbkrConnectionPump(c, 12U) == UMI_STATUS_OK);
        CHECK(!strcmp(c->snapshot.positions[0].quantity, "3.5") && c->snapshot.providerCode == 0);
        return 0;
    }
    UmiIbkrConnectionSnapshot previous; CHECK(UmiIbkrConnectionCopy(c, &previous) == UMI_STATUS_OK);
    const char *account = !strcmp(name, "alias") ? c->snapshot.selectedAccount : "DU456";
    CHECK(Refresh(f, account, 12U) == 0);
    CHECK(c->accountSummaryRequest == summary + 2U && c->accountPositionsRequest == positions + 2U);
    CHECK(c->snapshot.valueCount == 0U && c->snapshot.positionCount == 0U);
    CHECK(!c->snapshot.summaryComplete && !c->snapshot.positionsComplete);
    CHECK(!c->snapshot.summaryAtMilliseconds && !c->snapshot.positionsAtMilliseconds);
    CHECK(!strcmp(c->snapshot.selectedAccount, !strcmp(name, "alias") ? "DU123" : "DU456"));
    CHECK(previous.valueCount == 1U && previous.positionCount == 1U && previous.summaryComplete);
    if (!strcmp(name, "retired-replies")) {
        CHECK(Value(f, summary, "DU456", "999") == 0);
        CHECK(Position(f, positions, "DU456", "99", "") == 0);
        CHECK(End(f, true, summary) == 0); CHECK(End(f, false, positions) == 0);
        FEED(f, "4", "2", "36000", "321", "Old summary");
        CHECK(UmiIbkrConnectionPump(c, 13U) == UMI_STATUS_OK);
        CHECK(!c->snapshot.valueCount && !c->snapshot.positionCount);
        CHECK(!c->snapshot.summaryComplete && !c->snapshot.positionsComplete);
    } else if (!strcmp(name, "report-choice")) {
        CHECK(Complete(f, 13U) == 0);
        CHECK(UmiIbkrConnectionRefreshAccountReport(c, "DU123", UMI_IBKR_ACCOUNT_CURRENCY,
            "GBP", 14U) == UMI_STATUS_OK);
        CHECK(c->accountSummaryRequest == summary + 4U);
    } else CHECK(!strcmp(name, "repeat") || !strcmp(name, "alias") || !strcmp(name, "snapshot"));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *fixture = New(); if (!fixture) return 1;
    int result = Run(fixture, argv[1]); Delete(fixture); return result;
}
