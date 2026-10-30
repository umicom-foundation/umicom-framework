/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_time_sales.c
 *
 * PURPOSE:
 *   Exercise tick framing, atomic requests, decimal retention and stream lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "observation_fixture.h"
#include <limits.h>

static UmiIbkrTimeSalesQuery Query(UmiIbkrTimeSalesKind kind)
{
    UmiIbkrTimeSalesQuery q = {0};
    q.contract.contractId = 123U;
    strcpy(q.contract.exchange, "SMART");
    q.kind = kind;
    return q;
}
static int Trade(Fixture *f, uint32_t request, const char *price, const char *size)
{
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    FEED(f, "99", id, "1", "1791363600", price, size, "0", "TEST", "");
    return 0;
}
static int Wire(Fixture *f, const char *mode)
{
    UmiIbkrTimeSalesKind kind = UMI_IBKR_TICKS_LAST;
    const char *name = "Last";
    if (!strcmp(mode, "wire-all-last")) { kind = UMI_IBKR_TICKS_ALL_LAST; name = "AllLast"; }
    if (!strcmp(mode, "wire-bid-ask")) { kind = UMI_IBKR_TICKS_BID_ASK; name = "BidAsk"; }
    if (!strcmp(mode, "wire-midpoint")) { kind = UMI_IBKR_TICKS_MIDPOINT; name = "MidPoint"; }
    UmiIbkrTimeSalesQuery q = Query(kind);
    q.ignoreSize = kind == UMI_IBKR_TICKS_BID_ASK;
    uint32_t request = 0;
    CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    size_t offset = f->outSize;
    f->writeStep = 1U;
    for (uint64_t now = 11U; f->c->txSize && now < 1000U; ++now)
        CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
    CHECK(!f->c->txSize);
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *expected[] = {"97", id, "123", "", "", "", "0", "", "", "SMART",
                             "", "", "", "", name, "0", q.ignoreSize ? "1" : "0"};
    CHECK(ObserveWire(f, offset, expected, 17U) == 0);
    return 0;
}
static int Lifecycle(Fixture *f, const char *mode)
{
    UmiIbkrTimeSalesQuery q = Query(UMI_IBKR_TICKS_LAST);
    uint32_t request = 999U;
    if (!strcmp(mode, "request-atomic")) {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        uint32_t before = f->c->nextQuoteRequest;
        CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(request == 999U && before == f->c->nextQuoteRequest &&
              !f->c->timeSales[0] && !f->c->timeSalesRequested);
        f->c->txSize = 0;
    } else if (!strcmp(mode, "request-overflow")) {
        f->c->nextQuoteRequest = INT_MAX;
        CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(request == 999U && !f->c->timeSales[0]);
        return 0;
    }
    CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    UmiIbkrTimeSalesStore *store = UmiIbkrTimeSalesFind(f->c, request);
    CHECK(store && store->snapshot.needsCancel);
    if (!strcmp(mode, "capacity")) {
        for (uint32_t i = 1; i < UMI_IBKR_TIME_SALES_STREAM_LIMIT; ++i) {
            q.contract.contractId = 123U + i;
            uint32_t next = 0;
            CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 10U + 15000U * i, &next) == UMI_STATUS_OK);
            CHECK(next > request);
        }
        q.contract.contractId = 999U;
        uint32_t next = 0;
        CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 60010U, &next) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!next);
        return 0;
    }
    if (!strcmp(mode, "request-identity")) {
        uint32_t quote = 0;
        CHECK(UmiIbkrQuoteSubscribe(f->c, &q.contract, 11U, &quote) == UMI_STATUS_OK);
        CHECK(quote > request);
        return 0;
    }
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    size_t offset = f->outSize;
    if (!strcmp(mode, "cancel-atomic")) {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrTimeSalesCancel(f->c, request, 12U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(store->snapshot.active && store->snapshot.needsCancel);
        f->c->txSize = 0;
    }
    CHECK(UmiIbkrTimeSalesCancel(f->c, request, 12U) == UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *cancel[] = {"98", id};
    CHECK(ObserveWire(f, offset, cancel, 2U) == 0);
    CHECK(store->snapshot.cancelled && !store->snapshot.active && !store->snapshot.needsCancel);
    if (!strcmp(mode, "pacing") || !strcmp(mode, "retired")) {
        uint32_t next = 0;
        CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 15009U, &next) == UMI_STATUS_BUSY);
        CHECK(!next);
        CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 15010U, &next) == UMI_STATUS_OK);
        CHECK(next > request && !UmiIbkrTimeSalesFind(f->c, request));
        if (!strcmp(mode, "retired")) {
            /* Advance only the fixture clock observation for this identity
             * check; the late event still traverses the connection pump. */
            CHECK(Trade(f, request, "12.5", "1") == 0);
            f->c->snapshot.clockReceivedAtMilliseconds = 15010U;
            CHECK(UmiIbkrConnectionPump(f->c, 15011U) == UMI_STATUS_OK);
            CHECK(UmiIbkrTimeSalesFind(f->c, next)->snapshot.count == 0U);
        }
    }
    return 0;
}
static int Decode(Fixture *f, const char *mode)
{
    UmiIbkrTimeSalesQuery q = Query(UMI_IBKR_TICKS_LAST);
    if (!strcmp(mode, "all-last")) q.kind = UMI_IBKR_TICKS_ALL_LAST;
    if (!strcmp(mode, "bid-ask")) q.kind = UMI_IBKR_TICKS_BID_ASK;
    if (!strcmp(mode, "midpoint")) q.kind = UMI_IBKR_TICKS_MIDPOINT;
    uint32_t request = 0;
    CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"99", id, "1", "1791363600", "12.5", "0.25", "0", "TEST", ""};
    size_t count = 9U;
    if (!strcmp(mode, "all-last")) { fields[2] = "2"; fields[6] = "2"; fields[8] = "ABC"; }
    if (!strcmp(mode, "bid-ask")) {
        fields[2] = "3"; fields[4] = "12.5"; fields[5] = "12.75";
        fields[6] = "2.5"; fields[7] = "4.125"; fields[8] = "3";
    }
    if (!strcmp(mode, "midpoint")) { fields[2] = "4"; fields[4] = "12.625"; count = 5U; }
    if (!strcmp(mode, "negative-price")) fields[4] = "-0.25";
    if (!strcmp(mode, "precision")) fields[4] = "0.123456789012345678901234567890123456789";
    if (!strcmp(mode, "halt")) { fields[4] = "0"; fields[5] = "0"; fields[6] = "1"; }
    if (!strcmp(mode, "bad-price")) fields[4] = "nan";
    if (!strcmp(mode, "bad-size")) fields[5] = "-1";
    if (!strcmp(mode, "bad-flags")) fields[6] = "4";
    if (!strcmp(mode, "bad-time")) fields[3] = "18446744073709551616";
    if (!strcmp(mode, "bad-kind")) fields[2] = "3";
    if (!strcmp(mode, "missing-field")) count = 8U;
    if (!strcmp(mode, "invalid-utf8")) fields[7] = "\xc0\xaf";
    if (!strcmp(mode, "foreign")) fields[1] = "99999";
    if (!strcmp(mode, "counter-overflow"))
        UmiIbkrTimeSalesFind(f->c, request)->snapshot.receivedEvents = UINT64_MAX;
    if (!strcmp(mode, "malformed-retained")) {
        CHECK(Trade(f, request, "12.5", "0.25") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(Trade(f, request, "nan", "9") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_PARSE_ERROR);
        UmiIbkrTimeSalesSnapshot retained;
        UmiIbkrTimeSalesEvent retainedEvent;
        CHECK(UmiIbkrTimeSalesCopy(f->c, request, 12U, 100U, &retained) == UMI_STATUS_OK);
        CHECK(retained.failed && retained.stale && retained.count == 1U && retained.receivedEvents == 1U);
        CHECK(UmiIbkrTimeSalesEventCopy(f->c, request, 0U, &retainedEvent) == UMI_STATUS_OK);
        CHECK(!strcmp(retainedEvent.price.reportedText, "12.5") && retainedEvent.sequence == 1U);
        return 0;
    }
    CHECK(Feed(f, fields, count) == 0);
    if (!strcmp(mode, "fragmented-read")) f->readStep = 1U;
    UmiStatus expected = UMI_STATUS_OK;
    if (!strncmp(mode, "bad-", 4U) || !strcmp(mode, "missing-field") || !strcmp(mode, "invalid-utf8"))
        expected = UMI_STATUS_PARSE_ERROR;
    if (!strcmp(mode, "counter-overflow")) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == expected);
    UmiIbkrTimeSalesSnapshot snapshot;
    CHECK(UmiIbkrTimeSalesCopy(f->c, request, 11U, 100U, &snapshot) == UMI_STATUS_OK);
    if (expected != UMI_STATUS_OK) {
        CHECK(snapshot.failed && snapshot.stale && snapshot.count == 0U);
        return 0;
    }
    if (!strcmp(mode, "foreign")) {
        CHECK(snapshot.count == 0U && snapshot.stale);
        return 0;
    }
    UmiIbkrTimeSalesEvent event;
    CHECK(snapshot.count == 1U && !snapshot.stale);
    CHECK(UmiIbkrTimeSalesEventCopy(f->c, request, 0U, &event) == UMI_STATUS_OK);
    CHECK(event.sequence == 1U && event.receivedAtMilliseconds == 11U);
    if (!strcmp(mode, "bid-ask")) {
        CHECK(!strcmp(event.bidPrice.reportedText, "12.5"));
        CHECK(!strcmp(event.askPrice.reportedText, "12.75"));
        CHECK(!strcmp(event.bidSize.reportedText, "2.5"));
        CHECK(!strcmp(event.askSize.reportedText, "4.125") && event.flags == 3U);
    } else {
        CHECK(!strcmp(event.price.reportedText, fields[4]));
    }
    if (!strcmp(mode, "all-last")) CHECK(event.flags == 2U && !strcmp(event.specialConditions, "ABC"));
    if (!strcmp(mode, "halt")) CHECK(event.flags == 1U && !strcmp(event.size.reportedText, "0"));
    if (!strcmp(mode, "precision")) CHECK(!event.price.exact);
    if (!strcmp(mode, "same-tick") || !strcmp(mode, "rolling")) {
        unsigned repeats = !strcmp(mode, "rolling") ? UMI_IBKR_TIME_SALES_EVENT_LIMIT + 1U : 1U;
        for (unsigned i = 0; i < repeats; ++i) {
            CHECK(Trade(f, request, "12.5", "0.25") == 0);
            CHECK(UmiIbkrConnectionPump(f->c, 12U + i) == UMI_STATUS_OK);
        }
        CHECK(UmiIbkrTimeSalesCopy(f->c, request, 12U + repeats, 100U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.receivedEvents == 1U + repeats);
        CHECK(snapshot.droppedEvents == (!strcmp(mode, "rolling") ? 2U : 0U));
        CHECK(UmiIbkrTimeSalesEventCopy(f->c, request, 0U, &event) == UMI_STATUS_OK);
        CHECK(event.sequence == (!strcmp(mode, "rolling") ? 3U : 1U));
    }
    if (!strcmp(mode, "stale")) {
        CHECK(UmiIbkrTimeSalesCopy(f->c, request, 111U, 100U, &snapshot) == UMI_STATUS_OK && !snapshot.stale);
        CHECK(UmiIbkrTimeSalesCopy(f->c, request, 112U, 100U, &snapshot) == UMI_STATUS_OK && snapshot.stale);
    }
    if (!strcmp(mode, "cancel-retained") || !strcmp(mode, "disconnect-retained") || !strcmp(mode, "provider-error")) {
        if (!strcmp(mode, "cancel-retained")) {
            CHECK(UmiIbkrTimeSalesCancel(f->c, request, 12U) == UMI_STATUS_OK);
            CHECK(Trade(f, request, "999", "9") == 0);
            CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
        } else if (!strcmp(mode, "disconnect-retained")) {
            UmiIbkrConnectionClose(f->c);
        } else {
            FEED(f, "4", "2", id, "354", "Missing subscription");
            CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
        }
        CHECK(UmiIbkrTimeSalesCopy(f->c, request, 14U, 100U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.stale && snapshot.count == 1U && !snapshot.active);
        CHECK(UmiIbkrTimeSalesEventCopy(f->c, request, 0U, &event) == UMI_STATUS_OK);
        CHECK(!strcmp(event.price.reportedText, "12.5"));
        if (!strcmp(mode, "provider-error")) {
            CHECK(snapshot.failed && snapshot.needsCancel && snapshot.providerCode == 354);
            CHECK(f->c->snapshot.state == UMI_IBKR_READY);
            CHECK(UmiIbkrTimeSalesCancel(f->c, request, 15U) == UMI_STATUS_OK);
        }
    }
    if (!strcmp(mode, "copy-invalid")) {
        event.sequence = 999U;
        CHECK(UmiIbkrTimeSalesEventCopy(f->c, request, 1U, &event) == UMI_STATUS_NOT_FOUND);
        CHECK(event.sequence == 999U);
        snapshot.requestId = 999U;
        CHECK(UmiIbkrTimeSalesCopy(f->c, request, 14U, 0U, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(snapshot.requestId == 999U);
        CHECK(UmiIbkrTimeSalesCopy(f->c, request, 10U, 100U, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *cases[] = {
        "query-invalid", "not-ready", "unsupported", "request-atomic", "request-overflow",
        "wire-last", "wire-all-last", "wire-bid-ask", "wire-midpoint",
        "cancel-wire", "cancel-atomic", "pacing", "capacity", "request-identity", "retired",
        "trade", "all-last", "bid-ask", "midpoint", "halt", "negative-price", "precision",
        "same-tick", "rolling", "stale", "cancel-retained", "disconnect-retained",
        "provider-error", "foreign", "bad-price", "bad-size", "bad-flags", "bad-time",
        "bad-kind", "missing-field", "invalid-utf8", "fragmented-read", "counter-overflow", "copy-invalid",
        "malformed-retained", "mutation-denied"
    };
    bool found = false;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(mode, cases[i])) found = true;
    if (!found) return 2;
    Fixture *f = New();
    CHECK(f);
    UmiIbkrTimeSalesQuery q = Query(UMI_IBKR_TICKS_LAST);
    uint32_t request = 999U;
    if (!strcmp(mode, "not-ready")) {
        CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 0U, &request) == UMI_STATUS_INVALID_STATE);
        CHECK(request == 999U);
    } else if (!strcmp(mode, "query-invalid")) {
        CHECK(UmiIbkrTimeSalesQueryValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        q.ignoreSize = true;
        CHECK(UmiIbkrTimeSalesQueryValidate(&q) == UMI_STATUS_INVALID_ARGUMENT);
        q = Query(UMI_IBKR_TICKS_LAST); q.contract.contractId = 0;
        CHECK(UmiIbkrTimeSalesQueryValidate(&q) == UMI_STATUS_INVALID_ARGUMENT);
        q = Query(UMI_IBKR_TICKS_LAST); memset(q.contract.exchange, 'x', sizeof q.contract.exchange);
        CHECK(UmiIbkrTimeSalesQueryValidate(&q) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        CHECK(Connect(f) == 0);
        if (!strcmp(mode, "unsupported")) {
            f->c->snapshot.protocolVersion = 177;
            CHECK(UmiIbkrTimeSalesRequest(f->c, &q, 10U, &request) == UMI_STATUS_NOT_IMPLEMENTED);
            CHECK(request == 999U);
        } else if (!strcmp(mode, "mutation-denied")) {
            const char *place[] = {"3", "1"}, *cancelOrder[] = {"4", "1"};
            const char *exercise[] = {"21", "1"}, *globalCancel[] = {"58", "1"};
            size_t queued = f->c->txSize;
            CHECK(UmiIbkrQueueFields(f->c, place, 2U) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(UmiIbkrQueueFields(f->c, cancelOrder, 2U) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(UmiIbkrQueueFields(f->c, exercise, 2U) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(UmiIbkrQueueFields(f->c, globalCancel, 2U) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(f->c->txSize == queued);
        } else if (!strncmp(mode, "wire-", 5U)) {
            CHECK(Wire(f, mode) == 0);
        } else if (!strncmp(mode, "request-", 8U) || !strcmp(mode, "cancel-wire") ||
                   !strcmp(mode, "cancel-atomic") || !strcmp(mode, "pacing") ||
                   !strcmp(mode, "capacity") || !strcmp(mode, "retired")) {
            CHECK(Lifecycle(f, mode) == 0);
        } else {
            CHECK(Decode(f, mode) == 0);
        }
    }
    Delete(f);
    return 0;
}
