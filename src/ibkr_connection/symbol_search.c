/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/symbol_search.c
 * PURPOSE: Own a paced, bounded symbol-search request and its immutable result identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
static bool SearchPending(const UmiIbkrConnection *c, uint64_t now)
{
    return c->symbolSearch.requestId != 0U && !c->symbolSearch.complete && !c->symbolSearch.failed &&
           now >= c->symbolSearch.requestedAtMilliseconds &&
           now - c->symbolSearch.requestedAtMilliseconds < c->options.timeoutMilliseconds;
}
bool UmiIbkrSymbolSearchAccepting(const UmiIbkrConnection *c, uint32_t request, uint64_t now)
{
    return request != 0U && c->symbolSearch.requestId == request && c->snapshot.state == UMI_IBKR_READY &&
           SearchPending(c, now);
}
UmiStatus UmiIbkrSymbolSearchRequest(UmiIbkrConnection *c, const char *pattern, uint64_t now, uint32_t *out)
{
    if (c == NULL || out == NULL || now < c->lastNow || !UmiIbkrText(pattern, 128U, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    bool meaningful = false;
    for (const char *p = pattern; *p; ++p)
        if (*p != ' ')
            meaningful = true;
    if (!meaningful)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (SearchPending(c, now))
        return UMI_STATUS_BUSY;
    /* The timestamp survives abandonment and completion, preventing rapid
     * button clicks from bypassing the provider search pacing requirement. */
    if (c->symbolSearch.requestId != 0U && now - c->symbolSearch.requestedAtMilliseconds < 1000U)
        return UMI_STATUS_BUSY;
    if (c->nextQuoteRequest >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char owned[128];
    strcpy(owned, pattern);
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    char id[24];
    snprintf(id, sizeof id, "%" PRIu32, request);
    const char *fields[] = {"81", id, owned};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 3U);
    if (status != UMI_STATUS_OK)
        return status;
    memset(&c->symbolSearch, 0, sizeof c->symbolSearch);
    c->symbolSearch.requestId = request;
    c->symbolSearch.requestedAtMilliseconds = now;
    strcpy(c->symbolSearch.pattern, owned);
    strcpy(c->symbolSearch.message, "Waiting for matching contract candidates.");
    c->nextQuoteRequest = request + 1U;
    c->lastNow = now;
    *out = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrSymbolSearchCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now,
                                  UmiIbkrSymbolSearchSnapshot *out)
{
    if (c == NULL || out == NULL || request == 0U || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->symbolSearch.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    *out = c->symbolSearch;
    if (!out->complete && !out->failed &&
        now - out->requestedAtMilliseconds >= c->options.timeoutMilliseconds)
    {
        out->failed = true;
        strcpy(out->message, "Symbol search timed out. Retry explicitly when ready.");
    }
    out->stale = out->failed || c->snapshot.state != UMI_IBKR_READY;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrSymbolSearchAbandon(UmiIbkrConnection *c, uint32_t request)
{
    if (c == NULL || request == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->symbolSearch.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    c->symbolSearch.failed = true;
    c->symbolSearch.stale = true;
    strcpy(c->symbolSearch.message,
           "Local search abandoned; market-data subscriptions and orders are unchanged.");
    return UMI_STATUS_OK;
}
bool UmiIbkrSymbolSearchProviderMessage(UmiIbkrConnection *c, const char *id, int code, const char *text)
{
    (void)text;
    uint64_t request;
    if (!UmiIbkrUnsigned(id, &request) || request != c->symbolSearch.requestId || request == 0U ||
        c->symbolSearch.complete || c->symbolSearch.failed)
        return false;
    c->symbolSearch.failed = true;
    c->symbolSearch.stale = true;
    c->symbolSearch.providerCode = code;
    snprintf(c->symbolSearch.message, sizeof c->symbolSearch.message,
             "Provider refused symbol search (code %d).", code);
    return true;
}
