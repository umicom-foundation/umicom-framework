/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/realtime_bars.c
 * PURPOSE: Manage subscription identity, pacing, cancellation and retained rolling windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
UmiStatus UmiIbkrRealtimeQueryValidate(const UmiIbkrRealtimeQuery *q)
{
    if (!q || !q->contract.contractId || q->contract.contractId > (uint32_t)INT_MAX ||
        !UmiIbkrText(q->contract.exchange, sizeof q->contract.exchange, false) ||
        !UmiIbkrHistoricalDataSetting(q->dataKind))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiIbkrRealtimeStore *UmiIbkrRealtimeFind(const UmiIbkrConnection *c, uint32_t request)
{
    for (size_t i = 0; i < UMI_IBKR_REALTIME_STREAM_LIMIT; ++i)
        if (c->realtime[i] && c->realtime[i]->snapshot.requestId == request)
            return c->realtime[i];
    return NULL;
}
UmiStatus UmiIbkrRealtimeRequest(UmiIbkrConnection *c, const UmiIbkrRealtimeQuery *q, uint64_t now,
                                 uint32_t *out)
{
    if (!c || !out || now < c->lastNow || UmiIbkrRealtimeQueryValidate(q) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (c->barRequested && now - c->lastBarRequestAt < UMI_IBKR_BAR_REQUEST_INTERVAL_MS)
        return UMI_STATUS_BUSY;
    size_t slot = UMI_IBKR_REALTIME_STREAM_LIMIT;
    for (size_t i = 0; i < UMI_IBKR_REALTIME_STREAM_LIMIT; ++i)
    {
        const UmiIbkrRealtimeStore *s = c->realtime[i];
        if ((!s || !s->snapshot.needsCancel) && slot == UMI_IBKR_REALTIME_STREAM_LIMIT)
            slot = i;
        if (s && s->snapshot.needsCancel && s->snapshot.query.contract.contractId == q->contract.contractId &&
            !strcmp(s->snapshot.query.contract.exchange, q->contract.exchange) &&
            s->snapshot.query.dataKind == q->dataKind && s->snapshot.query.regularHours == q->regularHours)
            return UMI_STATUS_BUSY;
    }
    if (slot == UMI_IBKR_REALTIME_STREAM_LIMIT || c->nextQuoteRequest >= (uint32_t)INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIbkrRealtimeStore *next = calloc(1, sizeof *next);
    if (!next)
        return UMI_STATUS_OUT_OF_MEMORY;
    /* Match the shared next-unused sequence used by quotes and contract lookup.
     * The former last-issued convention could reuse an ID across request types;
     * retain it for review while keeping every observation owner distinct. */
#if 0
    uint32_t request = c->nextQuoteRequest + 1U;
#endif
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    char id[32], contract[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    (void)snprintf(contract, sizeof contract, "%u", (unsigned)q->contract.contractId);
    /* A resolved conId and route are supplied without guessing asset metadata.
     * Internal-use options remain empty; the broker decides route permissions. */
    const char *fields[] = {"50",
                            "3",
                            id,
                            contract,
                            "",
                            "",
                            "",
                            "0",
                            "",
                            "",
                            q->contract.exchange,
                            "",
                            "",
                            "",
                            "",
                            "5",
                            UmiIbkrHistoricalDataSetting(q->dataKind),
                            q->regularHours ? "1" : "0",
                            ""};
    UmiStatus status = UmiIbkrQueueFields(c, fields, sizeof fields / sizeof fields[0]);
    if (status != UMI_STATUS_OK)
    {
        free(next);
        return status;
    }
    next->snapshot.requestId = request;
    next->snapshot.query = *q;
    next->snapshot.active = true;
    next->snapshot.needsCancel = true;
    next->snapshot.stale = true;
    next->snapshot.requestedAtMilliseconds = now;
    strcpy(next->snapshot.message, "Subscribed; waiting for a new five-second bar.");
    /* Queue acceptance is the commit point. Failure leaves the old slot and
     * caller's request ID untouched, including any retained cancelled bars. */
    free(c->realtime[slot]);
    c->realtime[slot] = next;
    /* Reserve the following ID only after the complete request is queued.
     * The previous assignment is retained to explain the identity correction. */
#if 0
    c->nextQuoteRequest = request;
#endif
    c->nextQuoteRequest = request + 1U;
    c->lastNow = now;
    c->barRequested = true;
    c->lastBarRequestAt = now;
    *out = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrRealtimeCancel(UmiIbkrConnection *c, uint32_t request, uint64_t now)
{
    if (!c || !request || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrRealtimeStore *s = UmiIbkrRealtimeFind(c, request);
    if (!s)
        return UMI_STATUS_NOT_FOUND;
    if (c->snapshot.state != UMI_IBKR_READY || !s->snapshot.needsCancel)
        return UMI_STATUS_INVALID_STATE;
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"51", "1", id};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 3U);
    if (status != UMI_STATUS_OK)
        return status;
    s->snapshot.active = false;
    s->snapshot.needsCancel = false;
    s->snapshot.cancelled = true;
    s->snapshot.stale = true;
    c->lastNow = now;
    strcpy(s->snapshot.message, "Subscription cancelled locally; retained bars are stale.");
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrRealtimeCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t age,
                              UmiIbkrRealtimeSnapshot *out)
{
    if (!c || !out || !request || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrRealtimeStore *store = UmiIbkrRealtimeFind(c, request);
    if (!store)
        return UMI_STATUS_NOT_FOUND;
    UmiIbkrRealtimeSnapshot s = store->snapshot;
    s.stale = c->snapshot.state != UMI_IBKR_READY || !s.active || s.failed || s.cancelled || !s.count ||
              now - s.receivedAtMilliseconds > age;
    *out = s;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrRealtimeBarCopy(const UmiIbkrConnection *c, uint32_t request, size_t index,
                                 UmiIbkrRealtimeBar *out)
{
    if (!c || !out || !request)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrRealtimeStore *s = UmiIbkrRealtimeFind(c, request);
    if (!s || index >= s->snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    *out = s->bars[(s->first + index) % UMI_IBKR_REALTIME_BAR_LIMIT];
    return UMI_STATUS_OK;
}
void UmiIbkrRealtimeConnectionClosed(UmiIbkrConnection *c)
{
    for (size_t i = 0; i < UMI_IBKR_REALTIME_STREAM_LIMIT; ++i)
    {
        if (!c->realtime[i])
            continue;
        UmiIbkrRealtimeSnapshot *s = &c->realtime[i]->snapshot;
        if (s->needsCancel)
        {
            s->active = false;
            s->needsCancel = false;
            s->failed = true;
            s->stale = true;
            strcpy(s->message, "Connection closed; explicitly resubscribe after reconnecting.");
        }
    }
}
bool UmiIbkrRealtimeProviderMessage(UmiIbkrConnection *c, const char *id, int code, const char *message)
{
    uint64_t request;
    if (!UmiIbkrUnsigned(id, &request) || request > (uint64_t)UINT32_MAX)
        return false;
    UmiIbkrRealtimeStore *store = UmiIbkrRealtimeFind(c, (uint32_t)request);
    if (!store)
        return false;
    UmiIbkrRealtimeSnapshot *s = &store->snapshot;
    if (s->active)
    {
        s->active = false;
        s->failed = true;
        s->stale = true;
        s->providerCode = code;
        /* A bust notification invalidates this series. Keep the subscription
         * cancellable and the old rows inspectable; never silently repair prices. */
        if (code == 10225)
            strcpy(s->message, "Broker reported a bust event; cancel and explicitly request a new series.");
        else if (strlen(message) >= sizeof s->message)
            strcpy(s->message, "Broker stopped this series; diagnostic exceeds the display limit.");
        else
            strcpy(s->message, message);
    }
    return true;
}
