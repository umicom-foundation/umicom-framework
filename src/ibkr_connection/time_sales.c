/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/time_sales.c
 *
 * PURPOSE:
 *   Manage tick subscriptions, pacing, cancellation and retained observation ownership.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TickName(UmiIbkrTimeSalesKind kind)
{
    switch (kind) {
    case UMI_IBKR_TICKS_LAST: return "Last";
    case UMI_IBKR_TICKS_ALL_LAST: return "AllLast";
    case UMI_IBKR_TICKS_BID_ASK: return "BidAsk";
    case UMI_IBKR_TICKS_MIDPOINT: return "MidPoint";
    default: return NULL;
    }
}

UmiStatus UmiIbkrTimeSalesQueryValidate(const UmiIbkrTimeSalesQuery *q)
{
    if (!q || !q->contract.contractId || q->contract.contractId > (uint32_t)INT_MAX ||
        !UmiIbkrText(q->contract.exchange, sizeof q->contract.exchange, false) ||
        !TickName(q->kind) || (q->ignoreSize && q->kind != UMI_IBKR_TICKS_BID_ASK))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiIbkrTimeSalesStore *UmiIbkrTimeSalesFind(const UmiIbkrConnection *c, uint32_t request)
{
    for (size_t i = 0; i < UMI_IBKR_TIME_SALES_STREAM_LIMIT; ++i)
        if (c->timeSales[i] && c->timeSales[i]->snapshot.requestId == request)
            return c->timeSales[i];
    return NULL;
}

UmiStatus UmiIbkrTimeSalesRequest(UmiIbkrConnection *c, const UmiIbkrTimeSalesQuery *q,
                                 uint64_t now, uint32_t *out)
{
    if (!c || !out || now < c->lastNow || UmiIbkrTimeSalesQueryValidate(q) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (c->timeSalesRequested && now - c->lastTimeSalesRequestAt < UMI_IBKR_TIME_SALES_INTERVAL_MS)
        return UMI_STATUS_BUSY;

    size_t slot = UMI_IBKR_TIME_SALES_STREAM_LIMIT;
    for (size_t i = 0; i < UMI_IBKR_TIME_SALES_STREAM_LIMIT; ++i) {
        const UmiIbkrTimeSalesStore *old = c->timeSales[i];
        if ((!old || !old->snapshot.needsCancel) && slot == UMI_IBKR_TIME_SALES_STREAM_LIMIT)
            slot = i;
        if (old && old->snapshot.needsCancel &&
            old->snapshot.query.contract.contractId == q->contract.contractId &&
            old->snapshot.query.kind == q->kind &&
            !strcmp(old->snapshot.query.contract.exchange, q->contract.exchange))
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (slot == UMI_IBKR_TIME_SALES_STREAM_LIMIT || c->nextQuoteRequest >= (uint32_t)INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIbkrTimeSalesStore *next = calloc(1, sizeof *next);
    if (!next)
        return UMI_STATUS_OUT_OF_MEMORY;
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    char id[32], contract[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    (void)snprintf(contract, sizeof contract, "%u", (unsigned)q->contract.contractId);
    /* A resolved contract identity avoids symbol guessing. Zero historical
     * ticks selects streaming callbacks only, not an unhandled history reply. */
    const char *fields[] = {"97", id, contract, "", "", "", "0", "", "",
                           q->contract.exchange, "", "", "", "", TickName(q->kind),
                           "0", q->ignoreSize ? "1" : "0"};
    UmiStatus status = UmiIbkrQueueFields(c, fields, sizeof fields / sizeof fields[0]);
    if (status != UMI_STATUS_OK) {
        free(next);
        return status;
    }
    next->snapshot.requestId = request;
    next->snapshot.query = *q;
    next->snapshot.active = true;
    next->snapshot.needsCancel = true;
    next->snapshot.stale = true;
    next->snapshot.requestedAtMilliseconds = now;
    strcpy(next->snapshot.message, "Waiting for tick-by-tick observations.");
    /* Replace a retired slot only after the entire request enters the queue.
     * The shared next-unused sequence prevents cross-service callback reuse. */
    free(c->timeSales[slot]);
    c->timeSales[slot] = next;
    c->nextQuoteRequest = request + 1U;
    c->timeSalesRequested = true;
    c->lastTimeSalesRequestAt = now;
    c->lastNow = now;
    *out = request;
    return UMI_STATUS_OK;
}

UmiStatus UmiIbkrTimeSalesCancel(UmiIbkrConnection *c, uint32_t request, uint64_t now)
{
    if (!c || !request || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrTimeSalesStore *store = UmiIbkrTimeSalesFind(c, request);
    if (!store)
        return UMI_STATUS_NOT_FOUND;
    if (c->snapshot.state != UMI_IBKR_READY || !store->snapshot.needsCancel)
        return UMI_STATUS_INVALID_STATE;
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"98", id};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 2U);
    if (status != UMI_STATUS_OK)
        return status;
    store->snapshot.active = false;
    store->snapshot.needsCancel = false;
    store->snapshot.cancelled = true;
    store->snapshot.stale = true;
    strcpy(store->snapshot.message, "Tick subscription cancelled locally; retained events are stale.");
    c->lastNow = now;
    return UMI_STATUS_OK;
}

UmiStatus UmiIbkrTimeSalesCopy(const UmiIbkrConnection *c, uint32_t request,
                              uint64_t now, uint64_t age, UmiIbkrTimeSalesSnapshot *out)
{
    if (!c || !out || !request || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrTimeSalesStore *store = UmiIbkrTimeSalesFind(c, request);
    if (!store)
        return UMI_STATUS_NOT_FOUND;
    UmiIbkrTimeSalesSnapshot copy = store->snapshot;
    copy.stale = c->snapshot.state != UMI_IBKR_READY || !copy.active || copy.failed ||
                 copy.cancelled || !copy.count || now < copy.receivedAtMilliseconds ||
                 now - copy.receivedAtMilliseconds > age;
    *out = copy;
    return UMI_STATUS_OK;
}

UmiStatus UmiIbkrTimeSalesEventCopy(const UmiIbkrConnection *c, uint32_t request,
                                   size_t index, UmiIbkrTimeSalesEvent *out)
{
    if (!c || !out || !request)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrTimeSalesStore *store = UmiIbkrTimeSalesFind(c, request);
    if (!store || index >= store->snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    *out = store->events[(store->first + index) % UMI_IBKR_TIME_SALES_EVENT_LIMIT];
    return UMI_STATUS_OK;
}

void UmiIbkrTimeSalesClosed(UmiIbkrConnection *c)
{
    for (size_t i = 0; i < UMI_IBKR_TIME_SALES_STREAM_LIMIT; ++i) {
        if (!c->timeSales[i])
            continue;
        UmiIbkrTimeSalesSnapshot *s = &c->timeSales[i]->snapshot;
        if (s->needsCancel) {
            s->active = false;
            s->needsCancel = false;
            s->failed = true;
            s->stale = true;
            strcpy(s->message, "Connection closed; explicitly subscribe on the new connection.");
        }
    }
}

void UmiIbkrTimeSalesDestroy(UmiIbkrConnection *c)
{
    for (size_t i = 0; i < UMI_IBKR_TIME_SALES_STREAM_LIMIT; ++i) {
        free(c->timeSales[i]);
        c->timeSales[i] = NULL;
    }
}

bool UmiIbkrTimeSalesProviderMessage(UmiIbkrConnection *c, const char *id,
                                    int code, const char *message)
{
    uint64_t request;
    if (!UmiIbkrUnsigned(id, &request) || request > UINT32_MAX)
        return false;
    UmiIbkrTimeSalesStore *store = UmiIbkrTimeSalesFind(c, (uint32_t)request);
    if (!store)
        return false;
    UmiIbkrTimeSalesSnapshot *s = &store->snapshot;
    if (s->active) {
        s->active = false;
        s->failed = true;
        s->stale = true;
        s->providerCode = code;
        /* Leave cancellation available until its packet is queued. The
         * diagnostic does not prove that the broker released its subscription. */
        if (strlen(message) >= sizeof s->message)
            strcpy(s->message, "Broker diagnostic exceeds the display limit; text omitted.");
        else
            strcpy(s->message, message);
    }
    return true;
}
