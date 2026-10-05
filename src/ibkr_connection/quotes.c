/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/quotes.c
 * PURPOSE: Own read-only market data subscriptions and reject stale or unrelated callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Request identity belongs to the connection, not a table row or a UI pointer.
 * Cancelled slots may be reused, but their wire identifiers never are. */
static UmiIbkrQuoteSnapshot *Find(UmiIbkrConnection *connection, uint64_t request)
{
    for (size_t index = 0U; index < UMI_IBKR_QUOTE_CAPACITY; ++index)
        if (request != 0U && connection->quotes[index].requestId == request)
            return &connection->quotes[index];
    return NULL;
}

UmiStatus UmiIbkrQuoteSubscribe(UmiIbkrConnection *connection,
    const UmiIbkrQuoteContract *contract, uint64_t now, uint32_t *outRequest)
{
    if (connection == NULL || contract == NULL || outRequest == NULL ||
        contract->contractId == 0U || contract->contractId > INT_MAX ||
        !UmiIbkrText(contract->exchange, sizeof(contract->exchange), false) ||
        now < connection->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->snapshot.state != UMI_IBKR_READY) return UMI_STATUS_INVALID_STATE;
    UmiIbkrQuoteSnapshot *slot = NULL;
    for (size_t index = 0U; index < UMI_IBKR_QUOTE_CAPACITY; ++index) {
        UmiIbkrQuoteSnapshot *item = &connection->quotes[index];
        if (!item->subscribed) { if (slot == NULL) slot = item; continue; }
        if (item->contract.contractId == contract->contractId &&
            strcmp(item->contract.exchange, contract->exchange) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (slot == NULL || connection->nextQuoteRequest >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint32_t request = connection->nextQuoteRequest == 0U ? 36000U : connection->nextQuoteRequest;
    char requestText[24], contractText[24];
    (void)snprintf(requestText, sizeof(requestText), "%" PRIu32, request);
    (void)snprintf(contractText, sizeof(contractText), "%" PRIu32, contract->contractId);
    /* The negotiated legacy protocol uses the same contract fields for every
     * supported asset. Only conId/exchange are supplied; TWS resolves the exact
     * security and reports a refusal when the identity or entitlement is invalid.
     * Streaming flags are false for both ordinary and regulatory snapshots. */
    const char *type[] = {"59", "1", "1"};
    const char *fields[] = {"1", "11", requestText, contractText,
        "", "", "", "0", "", "", contract->exchange, "", "", "", "",
        "0", "", "0", "0", ""};
    size_t before = connection->txSize;
    UmiStatus status = UmiIbkrQueueFields(connection, type, 3U);
    if (status == UMI_STATUS_OK)
        status = UmiIbkrQueueFields(connection, fields, sizeof(fields) / sizeof(fields[0]));
    if (status != UMI_STATUS_OK) { connection->txSize = before; return status; }
    memset(slot, 0, sizeof(*slot));
    slot->requestId = request;
    slot->contract = *contract;
    slot->subscribed = true;
    slot->requestedAtMilliseconds = now;
    (void)snprintf(slot->message, sizeof(slot->message),
        "Subscribed data requested; waiting for provider type and observations.");
    connection->nextQuoteRequest = request + 1U;
    connection->lastNow = now;
    *outRequest = request;
    return UMI_STATUS_OK;
}

UmiStatus UmiIbkrQuoteCancel(UmiIbkrConnection *connection, uint32_t request)
{
    if (connection == NULL || request == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrQuoteSnapshot *quote = Find(connection, request);
    if (quote == NULL) return UMI_STATUS_NOT_FOUND;
    if (!quote->subscribed) return UMI_STATUS_OK;
    if (connection->snapshot.state != UMI_IBKR_READY) return UMI_STATUS_INVALID_STATE;
    char number[24];
    (void)snprintf(number, sizeof(number), "%" PRIu32, request);
    const char *fields[] = {"2", "2", number};
    UmiStatus status = UmiIbkrQueueFields(connection, fields, 3U);
    if (status != UMI_STATUS_OK) return status;
    quote->subscribed = false;
    (void)snprintf(quote->message, sizeof(quote->message), "Market data cancelled; retained values are stale.");
    return UMI_STATUS_OK;
}

/* Copying never refreshes receipt timestamps. Heartbeats and account responses
 * cannot make an old quote current, and a new ask cannot refresh an old bid. */
UmiStatus UmiIbkrQuoteCopy(const UmiIbkrConnection *connection, uint32_t request,
    uint64_t now, uint64_t maximumAge, UmiIbkrQuoteSnapshot *out)
{
    if (connection == NULL || out == NULL || request == 0U || maximumAge == 0U ||
        now < connection->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiIbkrQuoteSnapshot *found = NULL;
    for (size_t index = 0U; index < UMI_IBKR_QUOTE_CAPACITY; ++index)
        if (connection->quotes[index].requestId == request) found = &connection->quotes[index];
    if (found == NULL) return UMI_STATUS_NOT_FOUND;
    *out = *found;
    UmiIbkrQuoteValue *values[] = {&out->bid, &out->ask, &out->last,
        &out->bidSize, &out->askSize, &out->lastSize};
    for (size_t index = 0U; index < sizeof(values) / sizeof(values[0]); ++index) {
        UmiIbkrQuoteValue *value = values[index];
        value->stale = !out->subscribed || out->failed || !value->received || value->unavailable ||
            connection->snapshot.state != UMI_IBKR_READY || value->dataType == UMI_IBKR_DATA_UNKNOWN ||
            value->dataType != out->dataType || now < value->receivedAtMilliseconds ||
            now - value->receivedAtMilliseconds > maximumAge;
    }
    return UMI_STATUS_OK;
}

const char *UmiIbkrMarketDataTypeName(UmiIbkrMarketDataType type)
{
    switch (type) {
    case UMI_IBKR_DATA_REALTIME: return "real-time";
    case UMI_IBKR_DATA_FROZEN: return "frozen";
    case UMI_IBKR_DATA_DELAYED: return "delayed";
    case UMI_IBKR_DATA_DELAYED_FROZEN: return "delayed frozen";
    default: return "unconfirmed";
    }
}

/* Count unrelated and cancelled callbacks without assigning them to whichever
 * security happens to occupy the UI row now. */
static UmiStatus Ignored(UmiIbkrConnection *connection)
{
    if (connection->snapshot.ignoredFrames == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++connection->snapshot.ignoredFrames;
    return UMI_STATUS_OK;
}

/* Compare a bounded decimal to zero or minus one without locale-dependent
 * floating-point conversion. These are the provider's missing-price sentinels
 * when accompanied by zero size; other negative prices remain valid evidence. */
static bool Sentinel(const char *text, bool minusOne)
{
    const char *cursor = text;
    bool negative = *cursor == '-';
    if (*cursor == '-' || *cursor == '+') ++cursor;
    if (minusOne && !negative) return false;
    int fraction = 0, trailing = 0, nonzero = 0;
    bool point = false;
    while (*cursor != '\0' && *cursor != 'e' && *cursor != 'E') {
        char digit = *cursor++;
        if (digit == '.') { point = true; continue; }
        if (point) ++fraction;
        if (digit != '0') {
            if (!minusOne || digit != '1' || nonzero != 0) return false;
            ++nonzero; trailing = 0;
        } else if (nonzero != 0) ++trailing;
    }
    if (!minusOne) return true;
    if (nonzero == 0) return false;
    int exponent = 0;
    bool exponentNegative = false;
    if (*cursor != '\0') {
        ++cursor; exponentNegative = *cursor == '-';
        if (*cursor == '-' || *cursor == '+') ++cursor;
        while (*cursor != '\0') {
            if (exponent > 1000) return false;
            exponent = exponent * 10 + (*cursor++ - '0');
        }
    }
    return trailing + (exponentNegative ? -exponent : exponent) == fraction;
}

/* Keep malformed numeric text out of the observation record while retaining
 * valid decimal bytes exactly as the provider supplied them. */
static bool Decimal(const char *text)
{
    return UmiIbkrText(text, 96U, false) && UmiIbkrDecimalText(text);
}
/* Price or size, its type and its receipt time form one observation. Publish
 * those fields together only after the complete callback has been validated. */
static void Value(UmiIbkrQuoteValue *value, const char *text,
    UmiIbkrMarketDataType type, uint64_t now, bool unavailable)
{
    strcpy(value->text, text);
    value->receivedAtMilliseconds = now;
    value->dataType = type;
    value->received = true;
    value->unavailable = unavailable;
    value->stale = false;
}

/* Parse a complete callback before changing a value. A malformed known frame
 * fails the enclosing session; valid callbacks for other subscriptions are
 * ignored. Supported price/size fields are bid, ask and last only. */
UmiStatus UmiIbkrQuoteFrame(UmiIbkrConnection *connection, uint64_t message,
    char **fields, size_t count, uint64_t now)
{
    uint64_t request, tick;
    if (connection == NULL || fields == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (message != 1U && message != 2U && message != 58U) return UMI_STATUS_PARSE_ERROR;
    if (count < 3U || !UmiIbkrUnsigned(fields[2], &request) || request > INT_MAX)
        return UMI_STATUS_PARSE_ERROR;
    UmiIbkrQuoteSnapshot *quote = Find(connection, request);
    if (quote == NULL || !quote->subscribed || quote->failed) return Ignored(connection);
    if (message == 58U) {
        if (count != 4U || strcmp(fields[1], "1") != 0 ||
            !UmiIbkrUnsigned(fields[3], &tick) || tick < 1U || tick > 4U)
            return UMI_STATUS_PARSE_ERROR;
        quote->dataType = (UmiIbkrMarketDataType)tick;
        return UMI_STATUS_OK;
    }
    /* The official decoder consumes a callback revision before these fields.
     * Within our negotiated server range the complete field shape is stable;
     * older size/price callback revisions need not equal each other. */
    uint64_t revision;
    if (!UmiIbkrUnsigned(fields[1], &revision) || revision > INT_MAX ||
        (message == 1U && (count != 7U || revision < 3U)) ||
        (message == 2U && (count != 5U || revision < 1U)) ||
        !UmiIbkrUnsigned(fields[3], &tick)) return UMI_STATUS_PARSE_ERROR;
    UmiIbkrQuoteValue *price = NULL, *size = NULL;
    bool delayed = false;
    if (message == 1U) {
        uint64_t attributes;
        if (!Decimal(fields[4]) || !Decimal(fields[5]) ||
            !UmiIbkrUnsigned(fields[6], &attributes) || attributes > 7U)
            return UMI_STATUS_PARSE_ERROR;
        if (tick == 1U || tick == 66U) { price = &quote->bid; size = &quote->bidSize; }
        else if (tick == 2U || tick == 67U) { price = &quote->ask; size = &quote->askSize; }
        else if (tick == 4U || tick == 68U) { price = &quote->last; size = &quote->lastSize; }
        else return Ignored(connection);
        delayed = tick >= 66U;
    } else if (message == 2U) {
        if (!Decimal(fields[4])) return UMI_STATUS_PARSE_ERROR;
        if (tick == 0U || tick == 69U) size = &quote->bidSize;
        else if (tick == 3U || tick == 70U) size = &quote->askSize;
        else if (tick == 5U || tick == 71U) size = &quote->lastSize;
        else return Ignored(connection);
        delayed = tick >= 69U;
    } else return UMI_STATUS_PARSE_ERROR;
    UmiIbkrMarketDataType type = quote->dataType;
    /* A delayed tick is evidence of delay even before the type callback. Never
     * label it real-time merely because the owner requested subscribed data. */
    if (delayed) {
        if (type != UMI_IBKR_DATA_DELAYED_FROZEN) type = UMI_IBKR_DATA_DELAYED;
        quote->dataType = type;
    } else if (type == UMI_IBKR_DATA_DELAYED || type == UMI_IBKR_DATA_DELAYED_FROZEN) {
        type = UMI_IBKR_DATA_UNKNOWN;
    }
    if (price != NULL) {
        bool missing = Sentinel(fields[5], false) &&
            (Sentinel(fields[4], false) || Sentinel(fields[4], true));
        Value(price, fields[4], type, now, missing);
        Value(size, fields[5], type, now, fields[5][0] == '-');
    } else Value(size, fields[4], type, now, fields[4][0] == '-');
    (void)snprintf(quote->message, sizeof(quote->message), "Receiving provider observations; inspect each value's data type and age.");
    return UMI_STATUS_OK;
}

/* A permission/contract error scoped to a quote must not destroy already
 * received account observations. Stop accepting that subscription's ticks and
 * retain the diagnostic until the owner cancels it or reconnects. Global farm
 * and connection failures still follow the existing session failure policy. */
bool UmiIbkrQuoteProviderMessage(UmiIbkrConnection *connection,
    const char *requestText, int code, const char *message)
{
    uint64_t request;
    if (!UmiIbkrUnsigned(requestText, &request)) return false;
    UmiIbkrQuoteSnapshot *quote = Find(connection, request);
    /* A response for a cancelled/reused request must not disconnect the account. */
    if (request >= 36000U && request < connection->nextQuoteRequest &&
        (quote == NULL || !quote->subscribed)) return true;
    if (quote == NULL || !quote->subscribed) return false;
    quote->failed = true;
    quote->providerCode = code;
    if (strlen(message) > 440U)
        (void)snprintf(quote->message, sizeof(quote->message), "IBKR %d: provider diagnostic exceeds the display limit; text omitted.", code);
    else (void)snprintf(quote->message, sizeof(quote->message), "IBKR %d: %s", code, message);
    return true;
}
