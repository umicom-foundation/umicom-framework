/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_quote_data_type.c
 *
 * PURPOSE:
 *   Verify delivery-mode framing, shared-session admission and provider evidence without a broker.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

static UmiIbkrQuoteContract Contract(uint32_t identity)
{
    UmiIbkrQuoteContract contract = {0};
    contract.contractId = identity;
    strcpy(contract.exchange, "SMART");
    return contract;
}

/* Read the actual transport bytes using independent field expectations. The
 * request remains a stream with both snapshot flags false in every mode. */
static int Wire(Fixture *f, UmiIbkrMarketDataType type)
{
    UmiIbkrQuoteContract contract = Contract(265598U);
    uint32_t request = 999U;
    size_t before = f->outSize;
    CHECK(UmiIbkrQuoteSubscribeWithDataType(f->c, &contract, type, 10U, &request) == UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    const char *number = type == UMI_IBKR_DATA_REALTIME ? "1" :
        type == UMI_IBKR_DATA_FROZEN ? "2" : type == UMI_IBKR_DATA_DELAYED ? "3" : "4";
    const char *expected[][20] = {
        {"59", "1", number},
        {"1", "11", "36000", "265598", "", "", "", "0", "", "", "SMART",
            "", "", "", "", "0", "", "0", "0", ""}
    };
    const unsigned char *cursor = f->output + before;
    size_t remaining = f->outSize - before;
    for (size_t frame = 0U; frame < 2U; ++frame) {
        CHECK(remaining >= 4U);
        size_t length = ((size_t)cursor[0] << 24U) | ((size_t)cursor[1] << 16U) |
            ((size_t)cursor[2] << 8U) | cursor[3];
        CHECK(length <= remaining - 4U);
        const char *field = (const char *)cursor + 4U;
        size_t used = 0U, count = frame == 0U ? 3U : 20U;
        for (size_t i = 0U; i < count; ++i) {
            CHECK(used < length && memchr(field, '\0', length - used) != NULL);
            CHECK(strcmp(field, expected[frame][i]) == 0);
            size_t bytes = strlen(field) + 1U; field += bytes; used += bytes;
        }
        CHECK(used == length);
        cursor += length + 4U; remaining -= length + 4U;
    }
    CHECK(remaining == 0U);
    UmiIbkrQuoteSnapshot quote;
    CHECK(UmiIbkrQuoteCopy(f->c, request, 11U, 15U, &quote) == UMI_STATUS_OK);
    CHECK(quote.requestedDataType == type && quote.dataType == UMI_IBKR_DATA_UNKNOWN);
    CHECK(!quote.bid.received && quote.bid.stale);
    return 0;
}

static int Admission(Fixture *f, const char *mode)
{
    UmiIbkrQuoteContract contract = Contract(265598U);
    uint32_t request = 0U, second = 777U;
    if (strcmp(mode, "invalid") == 0) {
        size_t before = f->c->txSize;
        const UmiIbkrMarketDataType invalid[] = {UMI_IBKR_DATA_UNKNOWN,
            (UmiIbkrMarketDataType)-1, (UmiIbkrMarketDataType)5};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
            CHECK(UmiIbkrQuoteSubscribeWithDataType(f->c, &contract, invalid[i], 10U, &second) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(second == 777U && f->c->txSize == before && f->c->nextQuoteRequest == 0U);
        return 0;
    }
    if (strcmp(mode, "rollback") == 0) {
        /* The type frame fits but the contract frame does not. Existing queued
         * bytes must remain intact and the abandoned mode must never publish. */
        size_t before = UMI_IBKR_TX_LIMIT - 24U;
        memset(f->c->tx, 0x5a, before);
        f->c->txSize = before;
        CHECK(UmiIbkrQuoteSubscribeWithDataType(f->c, &contract, UMI_IBKR_DATA_DELAYED, 10U, &second) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->c->txSize == before && second == 777U && f->c->nextQuoteRequest == 0U);
        for (size_t i = 0U; i < before; ++i) CHECK(f->c->tx[i] == 0x5a);
        CHECK(f->c->quotes[0].requestId == 0U && !f->c->quotes[0].subscribed);
        f->c->txSize = 0U;
        return Wire(f, UMI_IBKR_DATA_REALTIME);
    }
    CHECK(UmiIbkrQuoteSubscribeWithDataType(f->c, &contract, UMI_IBKR_DATA_DELAYED, 10U, &request) == UMI_STATUS_OK);
    if (strcmp(mode, "duplicate") == 0) {
        CHECK(UmiIbkrQuoteSubscribeWithDataType(f->c, &contract, UMI_IBKR_DATA_REALTIME, 10U, &second) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(second == 777U);
        return 0;
    }
    if (strcmp(mode, "failed-conflict") == 0) {
        FEED(f, "4", "2", "36000", "354", "Synthetic permission refusal", "");
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(f->c->quotes[0].failed && f->c->quotes[0].subscribed);
    }
    if (strcmp(mode, "cancel-failure") == 0) {
        f->c->txSize = UMI_IBKR_TX_LIMIT - 1U;
        CHECK(UmiIbkrQuoteCancel(f->c, request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->c->quotes[0].subscribed);
    }
    if (strcmp(mode, "cancel-switch") == 0)
        CHECK(UmiIbkrQuoteCancel(f->c, request) == UMI_STATUS_OK);
    contract.contractId = 123U;
    size_t before = f->c->txSize;
    UmiIbkrQuoteSnapshot prior = f->c->quotes[0];
    if (strcmp(mode, "same-mode") == 0) {
        CHECK(UmiIbkrQuoteSubscribeWithDataType(f->c, &contract, UMI_IBKR_DATA_DELAYED, 20U, &second) == UMI_STATUS_OK);
        CHECK(second != request && f->c->quotes[1].requestedDataType == UMI_IBKR_DATA_DELAYED);
    } else if (strcmp(mode, "cancel-switch") == 0) {
        CHECK(UmiIbkrQuoteSubscribe(f->c, &contract, 20U, &second) == UMI_STATUS_OK);
        CHECK(second != request && f->c->quotes[0].requestedDataType == UMI_IBKR_DATA_REALTIME);
    } else {
        CHECK(UmiIbkrQuoteSubscribe(f->c, &contract, 20U, &second) == UMI_STATUS_BUSY);
        CHECK(second == 777U && f->c->txSize == before);
        CHECK(memcmp(&prior, &f->c->quotes[0], sizeof(prior)) == 0);
    }
    return 0;
}

static int Evidence(Fixture *f, const char *mode)
{
    UmiIbkrQuoteContract contract = Contract(265598U);
    uint32_t request = 0U;
    CHECK(UmiIbkrQuoteSubscribeWithDataType(f->c, &contract, UMI_IBKR_DATA_DELAYED, 10U, &request) == UMI_STATUS_OK);
    if (strcmp(mode, "provider-realtime") == 0) {
        FEED(f, "58", "1", "36000", "1");
        FEED(f, "1", "6", "36000", "1", "100.12500", "5", "0");
    } else if (strcmp(mode, "provider-frozen") == 0) {
        FEED(f, "58", "1", "36000", "4");
        FEED(f, "1", "6", "36000", "66", "100.12500", "5", "0");
    } else {
        /* A request alone cannot label an early ordinary tick as delayed. */
        FEED(f, "1", "6", "36000", "1", "100.12500", "5", "0");
    }
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    UmiIbkrQuoteSnapshot quote;
    CHECK(UmiIbkrQuoteCopy(f->c, request, 11U, 15U, &quote) == UMI_STATUS_OK);
    CHECK(quote.requestedDataType == UMI_IBKR_DATA_DELAYED);
    CHECK(strcmp(quote.bid.text, "100.12500") == 0);
    if (strcmp(mode, "provider-realtime") == 0)
        CHECK(quote.bid.dataType == UMI_IBKR_DATA_REALTIME && !quote.bid.stale);
    else if (strcmp(mode, "provider-frozen") == 0)
        CHECK(quote.bid.dataType == UMI_IBKR_DATA_DELAYED_FROZEN && !quote.bid.stale);
    else CHECK(quote.dataType == UMI_IBKR_DATA_UNKNOWN && quote.bid.stale);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *f = New();
    if (f == NULL) return 1;
    int result = Connect(f);
    if (result == 0) {
        if (strcmp(argv[1], "realtime") == 0) result = Wire(f, UMI_IBKR_DATA_REALTIME);
        else if (strcmp(argv[1], "frozen") == 0) result = Wire(f, UMI_IBKR_DATA_FROZEN);
        else if (strcmp(argv[1], "delayed") == 0) result = Wire(f, UMI_IBKR_DATA_DELAYED);
        else if (strcmp(argv[1], "delayed-frozen") == 0) result = Wire(f, UMI_IBKR_DATA_DELAYED_FROZEN);
        else if (strcmp(argv[1], "provider-realtime") == 0 || strcmp(argv[1], "provider-frozen") == 0 ||
            strcmp(argv[1], "unconfirmed") == 0) result = Evidence(f, argv[1]);
        else if (strcmp(argv[1], "invalid") == 0 || strcmp(argv[1], "rollback") == 0 ||
            strcmp(argv[1], "duplicate") == 0 || strcmp(argv[1], "same-mode") == 0 ||
            strcmp(argv[1], "conflict") == 0 || strcmp(argv[1], "failed-conflict") == 0 ||
            strcmp(argv[1], "cancel-failure") == 0 || strcmp(argv[1], "cancel-switch") == 0)
            result = Admission(f, argv[1]);
        else result = 2;
    }
    Delete(f);
    return result;
}
