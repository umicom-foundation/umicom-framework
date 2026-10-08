/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/option_chain.c
 * PURPOSE: Own a finite option-chain lookup without confusing partial results with completion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
UmiStatus UmiIbkrOptionChainQueryValidate(const UmiIbkrOptionChainQuery *q)
{
    if (!q || !q->underlyingContractId || q->underlyingContractId > INT_MAX ||
        !UmiIbkrText(q->underlyingSymbol, sizeof q->underlyingSymbol, false) ||
        !UmiIbkrText(q->underlyingSecurityType, sizeof q->underlyingSecurityType, false) ||
        !UmiIbkrText(q->exchange, sizeof q->exchange, true))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
bool UmiIbkrOptionChainPending(const UmiIbkrConnection *c, uint64_t now)
{
    const UmiIbkrOptionChainStore *s = c->optionChains;
    return s && !s->snapshot.complete && !s->snapshot.failed && !s->snapshot.abandoned &&
           now >= s->snapshot.requestedAtMilliseconds && now - s->snapshot.requestedAtMilliseconds < 60000U;
}
void UmiIbkrOptionChainStoreFree(UmiIbkrOptionChainStore *s)
{
    if (!s)
        return;
    for (size_t i = 0; i < UMI_IBKR_OPTION_CHAIN_LIMIT; ++i)
        free(s->items[i]);
    free(s);
}
UmiStatus UmiIbkrOptionChainRequest(UmiIbkrConnection *c, const UmiIbkrOptionChainQuery *q, uint64_t now,
                                    uint32_t *out)
{
    if (!c || !out || now < c->lastNow || UmiIbkrOptionChainQueryValidate(q) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (UmiIbkrOptionChainPending(c, now) ||
        (c->optionChainRequested && now - c->lastOptionChainRequestAt < 1000U))
        return UMI_STATUS_BUSY;
    if (c->nextQuoteRequest >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIbkrOptionChainStore *s = calloc(1, sizeof *s);
    if (!s)
        return UMI_STATUS_OUT_OF_MEMORY;
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    char id[32], contract[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    (void)snprintf(contract, sizeof contract, "%u", (unsigned)q->underlyingContractId);
    const char *fields[] = {"78", id, q->underlyingSymbol, q->exchange, q->underlyingSecurityType, contract};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 6U);
    if (status != UMI_STATUS_OK)
    {
        free(s);
        return status;
    }
    s->snapshot.query = *q;
    s->snapshot.requestId = request;
    s->snapshot.requestedAtMilliseconds = now;
    strcpy(s->snapshot.message, "Waiting for option definitions and their completion marker.");
    /* Replacing an abandoned or completed lookup releases only connection-owned
     * records. Callers holding copied chains retain independent values. */
    UmiIbkrOptionChainStoreFree(c->optionChains);
    c->optionChains = s;
    c->nextQuoteRequest = request + 1U;
    c->lastNow = now;
    c->optionChainRequested = true;
    c->lastOptionChainRequestAt = now;
    *out = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOptionChainCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now,
                                 UmiIbkrOptionChainSnapshot *out)
{
    if (!c || !request || !out || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->optionChains || c->optionChains->snapshot.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    *out = c->optionChains->snapshot;
    if (!out->complete && !out->failed && !out->abandoned && now - out->requestedAtMilliseconds >= 60000U)
    {
        out->failed = true;
        strcpy(out->message, "Option discovery timed out; partial definitions are not complete.");
    }
    out->stale = !out->complete || out->failed || out->abandoned || c->snapshot.state != UMI_IBKR_READY;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOptionChainItemCopy(const UmiIbkrConnection *c, uint32_t request, size_t index,
                                     UmiIbkrOptionChain *out)
{
    if (!c || !request || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->optionChains || c->optionChains->snapshot.requestId != request ||
        index >= c->optionChains->snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    *out = *c->optionChains->items[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOptionChainAbandon(UmiIbkrConnection *c, uint32_t request)
{
    if (!c || !request)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->optionChains || c->optionChains->snapshot.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    c->optionChains->snapshot.abandoned = true;
    c->optionChains->snapshot.stale = true;
    strcpy(c->optionChains->snapshot.message, "Local option lookup abandoned; late replies will be ignored.");
    return UMI_STATUS_OK;
}
bool UmiIbkrOptionChainProviderMessage(UmiIbkrConnection *c, const char *id, int code, const char *text)
{
    uint64_t request;
    if (!c->optionChains || !UmiIbkrUnsigned(id, &request) || request != c->optionChains->snapshot.requestId)
        return false;
    UmiIbkrOptionChainSnapshot *s = &c->optionChains->snapshot;
    if (!s->complete && !s->abandoned && !s->failed)
    {
        s->failed = true;
        s->stale = true;
        s->providerCode = code;
        strcpy(s->message, strlen(text) < sizeof s->message
                               ? text
                               : "Option discovery diagnostic exceeds the display limit.");
    }
    return true;
}
