/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_quotes.c
 * PURPOSE: Exercise real quote framing and ownership with the existing deterministic transport fixture.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>

static UmiIbkrQuoteContract Contract(uint32_t id)
{
    UmiIbkrQuoteContract contract = {0};
    contract.contractId = id;
    strcpy(contract.exchange, "SMART");
    return contract;
}
static int Subscribe(Fixture *fixture, uint32_t *request)
{
    UmiIbkrQuoteContract contract = Contract(265598U);
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, request) == UMI_STATUS_OK);
    CHECK(*request == 36000U);
    return 0;
}

/* Inspect independently specified wire fields, including the fee-bearing flag,
 * rather than reproducing the implementation's serializer in the expectation. */
static int Wire(Fixture *fixture)
{
    size_t before = fixture->outSize;
    uint32_t request = 0U;
    CHECK(Subscribe(fixture, &request) == 0);
    CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
    const unsigned char *cursor = fixture->output + before;
    size_t remaining = fixture->outSize - before;
    const char *expected[][20] = {
        {"59", "1", "1"},
        {"1", "11", "36000", "265598", "", "", "", "0", "", "", "SMART", "", "", "", "", "0", "", "0", "0", ""}
    };
    for (size_t frame = 0U; frame < 2U; ++frame) {
        CHECK(remaining >= 4U);
        size_t length = ((size_t)cursor[0] << 24U) | ((size_t)cursor[1] << 16U) |
            ((size_t)cursor[2] << 8U) | cursor[3];
        CHECK(length <= remaining - 4U);
        const char *field = (const char *)cursor + 4U;
        size_t used = 0U, count = frame == 0U ? 3U : 20U;
        for (size_t index = 0U; index < count; ++index) {
            CHECK(used < length && memchr(field, '\0', length - used) != NULL);
            CHECK(strcmp(field, expected[frame][index]) == 0);
            size_t bytes = strlen(field) + 1U; field += bytes; used += bytes;
        }
        CHECK(used == length);
        cursor += length + 4U; remaining -= length + 4U;
    }
    CHECK(remaining == 0U);
    const char *order[] = {"3", "1"}, *cancelOrder[] = {"4", "1"};
    CHECK(UmiIbkrQueueFields(fixture->c, order, 2U) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiIbkrQueueFields(fixture->c, cancelOrder, 2U) == UMI_STATUS_PERMISSION_DENIED);
    return 0;
}

static int Values(Fixture *fixture, const char *mode)
{
    uint32_t request = 0U;
    CHECK(Subscribe(fixture, &request) == 0);
    UmiIbkrQuoteSnapshot quote;
    CHECK(UmiIbkrQuoteCopy(fixture->c, request, 10U, 15U, &quote) == UMI_STATUS_OK);
    CHECK(quote.bid.stale && !quote.bid.received && quote.dataType == UMI_IBKR_DATA_UNKNOWN);
    if (strcmp(mode, "malformed") == 0) {
        FEED(fixture, "1", "6", "36000", "1", "not-a-number", "4", "0");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 11U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(!quote.bid.received && quote.bid.stale);
        return 0;
    }
    if (strcmp(mode, "field-versions") == 0) {
        FEED(fixture, "58", "1", "36000", "1");
        FEED(fixture, "1", "3", "36000", "1", "120.25", "10", "0");
        FEED(fixture, "2", "1", "36000", "0", "12");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 11U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(!quote.bid.stale && strcmp(quote.bidSize.text, "12") == 0);
        return 0;
    }
    if (strcmp(mode, "unknown-type") == 0) {
        FEED(fixture, "1", "6", "36000", "1", "120.25", "10", "0");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 11U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(quote.bid.received && quote.bid.stale && quote.bid.dataType == UMI_IBKR_DATA_UNKNOWN);
        return 0;
    }
    if (strcmp(mode, "delayed") == 0) {
        FEED(fixture, "1", "6", "36000", "66", "120.25", "10", "0");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 11U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(quote.bid.dataType == UMI_IBKR_DATA_DELAYED && quote.dataType == UMI_IBKR_DATA_DELAYED);
        FEED(fixture, "58", "1", "36000", "4");
        CHECK(UmiIbkrConnectionPump(fixture->c, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 12U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(quote.bid.stale && quote.dataType == UMI_IBKR_DATA_DELAYED_FROZEN);
        return 0;
    }
    FEED(fixture, "58", "1", "36000", "1");
    FEED(fixture, "1", "6", "36000", "1", "120.2500", "10.5", "1");
    FEED(fixture, "1", "6", "36000", "2", "120.50", "8", "0");
    FEED(fixture, "2", "6", "36000", "5", "1.125");
    CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
    CHECK(UmiIbkrQuoteCopy(fixture->c, request, 11U, 15U, &quote) == UMI_STATUS_OK);
    CHECK(!quote.bid.stale && strcmp(quote.bid.text, "120.2500") == 0);
    CHECK(strcmp(quote.bidSize.text, "10.5") == 0 && strcmp(quote.lastSize.text, "1.125") == 0);
    CHECK(!quote.last.received && quote.last.stale);
    if (strcmp(mode, "freshness") == 0) {
        FEED(fixture, "1", "6", "36000", "2", "121", "4", "0");
        CHECK(UmiIbkrConnectionPump(fixture->c, 30U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 30U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(quote.bid.stale && !quote.ask.stale && quote.bid.receivedAtMilliseconds == 11U);
        UmiIbkrConnectionClose(fixture->c);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 30U, 15U, &quote) == UMI_STATUS_OK && quote.ask.stale);
    } else if (strcmp(mode, "sentinels") == 0) {
        FEED(fixture, "1", "6", "36000", "1", "-10.00e-1", "0.0", "0");
        FEED(fixture, "1", "6", "36000", "2", "-1.0", "4", "0");
        CHECK(UmiIbkrConnectionPump(fixture->c, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 12U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(quote.bid.unavailable && quote.bid.stale && !quote.ask.unavailable);
    } else if (strcmp(mode, "frozen") == 0) {
        FEED(fixture, "58", "1", "36000", "2");
        FEED(fixture, "1", "6", "36000", "4", "120.1", "2", "0");
        CHECK(UmiIbkrConnectionPump(fixture->c, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 12U, 15U, &quote) == UMI_STATUS_OK);
        CHECK(quote.bid.stale && quote.last.dataType == UMI_IBKR_DATA_FROZEN);
    }
    return 0;
}

static int Ownership(Fixture *fixture, const char *mode)
{
    uint32_t request = 0U, next = 0U;
    CHECK(Subscribe(fixture, &request) == 0);
    UmiIbkrQuoteContract contract = Contract(265598U);
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &next) == UMI_STATUS_ALREADY_EXISTS && next == 0U);
    if (strcmp(mode, "capacity") == 0) {
        for (uint32_t index = 1U; index < UMI_IBKR_QUOTE_CAPACITY; ++index) {
            contract.contractId = index;
            CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &next) == UMI_STATUS_OK);
        }
        next = 77U; contract.contractId = 999U;
        CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &next) == UMI_STATUS_CAPACITY_EXCEEDED && next == 77U);
        return 0;
    }
    if (strcmp(mode, "queue-failure") == 0) {
        fixture->c->txSize = UMI_IBKR_TX_LIMIT - 1U;
        size_t before = fixture->c->txSize;
        contract.contractId = 999U; next = 88U;
        CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &next) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(next == 88U && fixture->c->txSize == before && fixture->c->nextQuoteRequest == 36001U);
        CHECK(UmiIbkrQuoteCancel(fixture->c, request) == UMI_STATUS_CAPACITY_EXCEEDED);
        UmiIbkrQuoteSnapshot quote;
        CHECK(UmiIbkrQuoteCopy(fixture->c, request, 10U, 10U, &quote) == UMI_STATUS_OK && quote.subscribed);
        return 0;
    }
    CHECK(UmiIbkrQuoteCancel(fixture->c, request) == UMI_STATUS_OK);
    CHECK(UmiIbkrQuoteCancel(fixture->c, request) == UMI_STATUS_OK);
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &next) == UMI_STATUS_OK && next != request);
    FEED(fixture, "1", "6", "36000", "1", "999", "5", "0");
    FEED(fixture, "4", "2", "36000", "300", "Late cancellation response", "");
    CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
    UmiIbkrQuoteSnapshot quote;
    CHECK(UmiIbkrQuoteCopy(fixture->c, next, 11U, 10U, &quote) == UMI_STATUS_OK && !quote.bid.received && !quote.failed);
    CHECK(UmiIbkrQuoteCopy(fixture->c, request, 11U, 10U, &quote) == UMI_STATUS_NOT_FOUND);
    CHECK(fixture->c->snapshot.state == UMI_IBKR_READY);
    return 0;
}

/* A quote permission error must preserve the independent account request and
 * its rows, while refusing to revive the failed quote with subsequent ticks. */
static int Refusal(Fixture *fixture)
{
    uint32_t request = 0U;
    CHECK(Subscribe(fixture, &request) == 0);
    CHECK(UmiIbkrConnectionReadAccount(fixture->c, "DU123", 10U) == UMI_STATUS_OK);
    CHECK(PositionFeed(fixture, "DU123", "2", "100") == 0);
    FEED(fixture, "4", "2", "36000", "354", "Market data permission required", "");
    FEED(fixture, "1", "6", "36000", "1", "999", "5", "0");
    CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
    CHECK(fixture->c->snapshot.state == UMI_IBKR_READY && fixture->c->snapshot.positionCount == 1U);
    UmiIbkrQuoteSnapshot quote;
    CHECK(UmiIbkrQuoteCopy(fixture->c, request, 11U, 10U, &quote) == UMI_STATUS_OK);
    CHECK(quote.failed && quote.providerCode == 354 && !quote.bid.received && quote.bid.stale);
    CHECK(strstr(quote.message, "permission") != NULL);
    CHECK(UmiIbkrQuoteCancel(fixture->c, request) == UMI_STATUS_OK);
    return 0;
}

static int Invalid(Fixture *fixture)
{
    UmiIbkrQuoteContract contract = Contract(0U);
    uint32_t request = 99U;
    size_t before = fixture->c->txSize;
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &request) == UMI_STATUS_INVALID_ARGUMENT);
    contract.contractId = UINT32_MAX;
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &request) == UMI_STATUS_INVALID_ARGUMENT);
    contract.contractId = 12U; memset(contract.exchange, 'x', sizeof(contract.exchange));
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &request) == UMI_STATUS_INVALID_ARGUMENT);
    strcpy(contract.exchange, "SMART\nBAD");
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &request) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(request == 99U && fixture->c->txSize == before);
    contract = Contract(12U);
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 1U, &request) == UMI_STATUS_INVALID_ARGUMENT);
    fixture->c->nextQuoteRequest = INT_MAX;
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
    UmiIbkrConnectionClose(fixture->c);
    CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &request) == UMI_STATUS_INVALID_STATE);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *fixture = New();
    if (fixture == NULL) return 1;
    int result = Connect(fixture);
    if (result == 0) {
        if (strcmp(argv[1], "wire") == 0) result = Wire(fixture);
        else if (strcmp(argv[1], "invalid") == 0) result = Invalid(fixture);
        else if (strcmp(argv[1], "refusal") == 0) result = Refusal(fixture);
        else if (strcmp(argv[1], "capacity") == 0 || strcmp(argv[1], "queue-failure") == 0 || strcmp(argv[1], "cancel-reuse") == 0)
            result = Ownership(fixture, argv[1]);
        else if (strcmp(argv[1], "field-versions") == 0 || strcmp(argv[1], "values") == 0 || strcmp(argv[1], "freshness") == 0 || strcmp(argv[1], "sentinels") == 0 ||
            strcmp(argv[1], "frozen") == 0 || strcmp(argv[1], "delayed") == 0 || strcmp(argv[1], "unknown-type") == 0 || strcmp(argv[1], "malformed") == 0)
            result = Values(fixture, argv[1]);
        else result = 2;
    }
    Delete(fixture);
    return result;
}
