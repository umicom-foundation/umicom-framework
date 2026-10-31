/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/option_computation.c
 *
 * PURPOSE:
 *   Decode option risk observations atomically within their quote subscription lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <string.h>

/* Compare provider sentinel values without locale-dependent floating point.
 * Normalisation is used only for -1/-2 availability markers; the original
 * decimal text remains intact. Bounded exponents cannot overflow this parser. */
static bool Unavailable(const char *text, char marker)
{
    if (*text++ != '-') return false;
    int fractional = 0, trailing = 0, nonzero = 0, exponent = 0;
    bool point = false;
    while (*text != '\0' && *text != 'e' && *text != 'E') {
        char digit = *text++;
        if (digit == '.') { point = true; continue; }
        if (point) ++fractional;
        if (digit != '0') {
            if (digit != marker || nonzero != 0) return false;
            ++nonzero;
            trailing = 0;
        } else if (nonzero != 0) ++trailing;
    }
    if (nonzero == 0) return false;
    if (*text != '\0') {
        ++text;
        bool negative = *text == '-';
        if (*text == '+' || *text == '-') ++text;
        while (*text != '\0') {
            if (exponent > 1000) return false;
            exponent = exponent * 10 + (*text++ - '0');
        }
        if (negative) exponent = -exponent;
    }
    return trailing + exponent == fractional;
}

static UmiStatus Ignore(UmiIbkrConnection *connection)
{
    if (connection->snapshot.ignoredFrames == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++connection->snapshot.ignoredFrames;
    return UMI_STATUS_OK;
}

UmiStatus UmiIbkrOptionComputationFrame(UmiIbkrConnection *connection,
    char **fields, size_t count, uint64_t now)
{
    if (connection == NULL || fields == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Protocol 156 replaces the callback revision with the volatility basis
     * attribute. The two shapes have equal full lengths but different offsets. */
    bool attributes = connection->snapshot.protocolVersion >= 156;
    size_t requestAt = attributes ? 1U : 2U;
    uint64_t request, tick, revision = 6U, attribute = 0U;
    if (count <= requestAt || !UmiIbkrUnsigned(fields[requestAt], &request) ||
        request > INT_MAX) return UMI_STATUS_PARSE_ERROR;
    UmiIbkrQuoteSnapshot *quote = NULL;
    for (size_t i = 0U; i < UMI_IBKR_QUOTE_CAPACITY; ++i)
        if (request != 0U && connection->quotes[i].requestId == request)
            quote = &connection->quotes[i];
    if (quote == NULL || !quote->subscribed || quote->failed) return Ignore(connection);
    if (count <= requestAt + 1U || !UmiIbkrUnsigned(fields[requestAt + 1U], &tick))
        return UMI_STATUS_PARSE_ERROR;
    bool delayed = tick >= 80U && tick <= 83U;
    if (!delayed && (tick < 10U || tick > 13U)) return Ignore(connection);
    size_t basis = (size_t)(delayed ? tick - 80U : tick - 10U);
    if (attributes) {
        if (count < 4U || !UmiIbkrUnsigned(fields[3], &attribute) || attribute > INT_MAX)
            return UMI_STATUS_PARSE_ERROR;
    } else if (!UmiIbkrUnsigned(fields[1], &revision) || revision == 0U || revision > INT_MAX)
        return UMI_STATUS_PARSE_ERROR;
    size_t measures = revision >= 6U ? 8U :
        (basis == UMI_IBKR_OPTION_MODEL ? 4U : 2U);
    if (count != 4U + measures) return UMI_STATUS_PARSE_ERROR;
    /* Validate the entire callback before replacing any existing calculation.
     * Optional old callback fields remain unreceived, not fabricated zeros. */
    for (size_t i = 0U; i < measures; ++i)
        if (!UmiIbkrDecimalText(fields[4U + i])) return UMI_STATUS_PARSE_ERROR;
    UmiIbkrMarketDataType type = quote->dataType;
    if (delayed) {
        if (type != UMI_IBKR_DATA_DELAYED_FROZEN) type = UMI_IBKR_DATA_DELAYED;
    } else if (type == UMI_IBKR_DATA_DELAYED || type == UMI_IBKR_DATA_DELAYED_FROZEN)
        type = UMI_IBKR_DATA_UNKNOWN;
    UmiIbkrOptionComputation result = {0};
    result.received = true;
    result.attributeReceived = attributes;
    result.tickAttribute = (uint32_t)attribute;
    const char markers[] = {'1', '2', '1', '1', '2', '2', '2', '1'};
    for (size_t i = 0U; i < measures; ++i) {
        UmiIbkrQuoteValue *value = &result.values[i];
        strcpy(value->text, fields[4U + i]);
        value->received = true;
        value->receivedAtMilliseconds = now;
        value->dataType = type;
        value->unavailable = Unavailable(value->text, markers[i]);
    }
    if (delayed) quote->dataType = type;
    quote->options[basis] = result;
    return UMI_STATUS_OK;
}

void UmiIbkrOptionComputationAge(UmiIbkrQuoteSnapshot *quote, bool connected,
    uint64_t now, uint64_t maximumAge)
{
    for (size_t basis = 0U; basis < UMI_IBKR_OPTION_BASIS_COUNT; ++basis)
        for (size_t measure = 0U; measure < UMI_IBKR_OPTION_MEASURE_COUNT; ++measure) {
            UmiIbkrQuoteValue *value = &quote->options[basis].values[measure];
            value->stale = !connected || !quote->subscribed || quote->failed ||
                !value->received || value->unavailable ||
                value->dataType == UMI_IBKR_DATA_UNKNOWN ||
                value->dataType != quote->dataType || now < value->receivedAtMilliseconds ||
                now - value->receivedAtMilliseconds > maximumAge;
        }
}
