/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/market_depth.c
 * PURPOSE: Validate and frame independent depth subscriptions while preserving their scope.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
static UmiIbkrDepthSnapshot *DepthFind(UmiIbkrConnection *c, uint32_t id)
{
    for (size_t i = 0U; i < UMI_IBKR_DEPTH_STREAM_LIMIT; ++i)
        if (id != 0U && c->depth[i].requestId == id)
            return &c->depth[i];
    return NULL;
}
static bool DepthSelectionValid(const UmiIbkrDepthSelection *s)
{
    if (s == NULL || s->contractId == 0U || s->contractId > INT_MAX || s->rows == 0U ||
        s->rows > UMI_IBKR_DEPTH_ROW_LIMIT || !UmiIbkrText(s->exchange, sizeof s->exchange, false))
        return false;
    const char *texts[] = {s->symbol,     s->securityType,    s->expiry,   s->strike,      s->right,
                           s->multiplier, s->primaryExchange, s->currency, s->localSymbol, s->tradingClass};
    const size_t capacities[] = {sizeof s->symbol,          sizeof s->securityType, sizeof s->expiry,
                                 sizeof s->strike,          sizeof s->right,        sizeof s->multiplier,
                                 sizeof s->primaryExchange, sizeof s->currency,     sizeof s->localSymbol,
                                 sizeof s->tradingClass};
    for (size_t i = 0U; i < sizeof texts / sizeof texts[0]; ++i)
        if (!UmiIbkrText(texts[i], capacities[i], true))
            return false;
    if (!strcmp(s->securityType, "BAG") || s->smartDepth != (strcmp(s->exchange, "SMART") == 0))
        return false;
    if (s->right[0] && strcmp(s->right, "C") && strcmp(s->right, "P"))
        return false;
    UmiDecimal strike, multiplier;
    if (s->strike[0] &&
        (UmiDecimalParseScientificExact(s->strike, strlen(s->strike), &strike) != UMI_STATUS_OK ||
         strike.coefficient < 0))
        return false;
    if (s->multiplier[0] &&
        (UmiDecimalParseScientificExact(s->multiplier, strlen(s->multiplier), &multiplier) != UMI_STATUS_OK ||
         multiplier.coefficient <= 0))
        return false;
    return true;
}
UmiStatus UmiIbkrDepthSubscribe(UmiIbkrConnection *c, const UmiIbkrDepthSelection *selection, uint64_t now,
                                uint32_t *out)
{
    if (c == NULL || out == NULL || now < c->lastNow || !DepthSelectionValid(selection))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiIbkrDepthSnapshot *slot = NULL;
    for (size_t i = 0U; i < UMI_IBKR_DEPTH_STREAM_LIMIT; ++i)
    {
        UmiIbkrDepthSnapshot *row = &c->depth[i];
        if (!row->subscribed)
        {
            if (slot == NULL)
                slot = row;
            continue;
        }
        if (row->selection.contractId == selection->contractId &&
            !strcmp(row->selection.exchange, selection->exchange))
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (slot == NULL || c->nextQuoteRequest >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Own the selection before reusing a cancelled slot. A failed queue cannot
     * erase a captured ladder or consume a request identity. */
    UmiIbkrDepthSelection owned = *selection;
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    char id[24], contract[24], rows[24];
    snprintf(id, sizeof id, "%u", (unsigned)request);
    snprintf(contract, sizeof contract, "%u", (unsigned)owned.contractId);
    snprintf(rows, sizeof rows, "%u", (unsigned)owned.rows);
    const char *fields[] = {"10",
                            "5",
                            id,
                            contract,
                            owned.symbol,
                            owned.securityType,
                            owned.expiry,
                            owned.strike[0] ? owned.strike : "0",
                            owned.right,
                            owned.multiplier,
                            owned.exchange,
                            owned.primaryExchange,
                            owned.currency,
                            owned.localSymbol,
                            owned.tradingClass,
                            rows,
                            owned.smartDepth ? "1" : "0",
                            ""};
    UmiStatus status = UmiIbkrQueueFields(c, fields, sizeof fields / sizeof fields[0]);
    if (status != UMI_STATUS_OK)
        return status;
    memset(slot, 0, sizeof *slot);
    slot->requestId = request;
    slot->selection = owned;
    slot->subscribed = true;
    slot->stale = true;
    slot->requestedAtMilliseconds = now;
    slot->revision = 1U;
    strcpy(slot->message, "Waiting for depth updates; no complete-book or execution guarantee is implied.");
    c->nextQuoteRequest = request + 1U;
    c->lastNow = now;
    *out = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrDepthCancel(UmiIbkrConnection *c, uint32_t request)
{
    if (c == NULL || request == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrDepthSnapshot *slot = DepthFind(c, request);
    if (slot == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (!slot->subscribed)
        return UMI_STATUS_OK;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (slot->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char id[24];
    snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"11", "1", id, slot->selection.smartDepth ? "1" : "0"};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 4U);
    if (status != UMI_STATUS_OK)
        return status;
    slot->subscribed = false;
    slot->stale = true;
    ++slot->revision;
    strcpy(slot->message, "Depth stream stopped; retained ladder is historical.");
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrDepthCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t age,
                           UmiIbkrDepthSnapshot *out)
{
    if (c == NULL || out == NULL || request == 0U || age == 0U || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiIbkrDepthSnapshot *slot = NULL;
    for (size_t i = 0U; i < UMI_IBKR_DEPTH_STREAM_LIMIT; ++i)
        if (c->depth[i].requestId == request)
            slot = &c->depth[i];
    if (slot == NULL)
        return UMI_STATUS_NOT_FOUND;
    *out = *slot;
    out->stale = !out->subscribed || out->failed || !out->received || c->snapshot.state != UMI_IBKR_READY ||
                 now < out->receivedAtMilliseconds || now - out->receivedAtMilliseconds > age;
    /* A change to one level does not refresh the receipt time of other levels. */
    for (size_t i = 0U; i < out->bidCount; ++i)
        out->bids[i].stale = out->stale || now < out->bids[i].receivedAtMilliseconds ||
                             now - out->bids[i].receivedAtMilliseconds > age;
    for (size_t i = 0U; i < out->askCount; ++i)
        out->asks[i].stale = out->stale || now < out->asks[i].receivedAtMilliseconds ||
                             now - out->asks[i].receivedAtMilliseconds > age;
    return UMI_STATUS_OK;
}
bool UmiIbkrDepthProviderMessage(UmiIbkrConnection *c, const char *id, int code, const char *message)
{
    (void)message;
    uint64_t request;
    if (!UmiIbkrUnsigned(id, &request) || request > UINT32_MAX)
        return false;
    UmiIbkrDepthSnapshot *slot = DepthFind(c, (uint32_t)request);
    if (slot == NULL || !slot->subscribed || slot->failed)
        return false;
    slot->providerCode = code;
    slot->stale = true;
    /* IBKR reset 317 is a command to discard positional state, not an ordinary
     * warning. Reusing old rows would apply later inserts to the wrong book. */
    if (code == 317 && slot->revision != UINT64_MAX && slot->resetCount != UINT64_MAX)
    {
        memset(slot->bids, 0, sizeof slot->bids);
        memset(slot->asks, 0, sizeof slot->asks);
        slot->bidCount = 0U;
        slot->askCount = 0U;
        slot->received = false;
        slot->receivedAtMilliseconds = 0U;
        ++slot->revision;
        ++slot->resetCount;
        strcpy(slot->message, "Provider reset the depth book. Waiting for new entries.");
    }
    else
    {
        slot->failed = true;
        if (slot->revision != UINT64_MAX)
            ++slot->revision;
        snprintf(slot->message, sizeof slot->message,
                 "Depth stream needs an explicit restart (provider code %d).", code);
    }
    return true;
}
