/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/historical_bars.c
 * PURPOSE: Own finite historical requests, cancellation, pacing and retained observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static bool TimedOut(const UmiIbkrConnection *c, uint64_t now)
{
    return c->history && c->history->snapshot.pending &&
           now >= c->history->snapshot.requestedAtMilliseconds &&
           now - c->history->snapshot.requestedAtMilliseconds >= UMI_IBKR_HISTORICAL_TIMEOUT_MS;
}
void UmiIbkrHistoricalExpire(UmiIbkrConnection *c, uint64_t now)
{
    if (!TimedOut(c, now))
        return;
    UmiIbkrHistoricalSnapshot *s = &c->history->snapshot;
    s->pending = false;
    s->failed = true;
    s->stale = true;
    (void)snprintf(s->message, sizeof s->message, "Historical capture timed out; late bars will be ignored.");
}
/* Closing the transport ends ownership of a pending response. Keep its query
 * and reason for inspection rather than leaving a disconnected request pending. */
void UmiIbkrHistoricalConnectionClosed(UmiIbkrConnection *c)
{
    if (!c->history || !c->history->snapshot.pending)
        return;
    UmiIbkrHistoricalSnapshot *s = &c->history->snapshot;
    s->pending = false;
    s->failed = true;
    s->stale = true;
    (void)snprintf(s->message, sizeof s->message,
                   "The connection closed before the historical capture completed.");
}
UmiStatus UmiIbkrHistoricalRequest(UmiIbkrConnection *c, const UmiIbkrHistoricalQuery *q, uint64_t now,
                                   uint32_t *out)
{
    if (!c || !out || now < c->lastNow || UmiIbkrHistoricalQueryValidate(q) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (c->history && c->history->snapshot.pending && !TimedOut(c, now))
        return UMI_STATUS_BUSY;
    /* This is a local throttle, not a promise about account-wide broker limits.
     * Replacing or cancelling a query cannot reset its pacing interval. */
    if (c->historyRequested && now - c->lastHistoryRequestAt < UMI_IBKR_HISTORICAL_REQUEST_INTERVAL_MS)
        return UMI_STATUS_BUSY;
    /* Share pacing with streaming bar requests; cancellation cannot bypass the
     * conservative connection-local interval. Other clients have separate limits. */
    if (c->barRequested && now-c->lastBarRequestAt<UMI_IBKR_BAR_REQUEST_INTERVAL_MS)
        return UMI_STATUS_BUSY;
    if (c->nextQuoteRequest >= (uint32_t)INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Match the shared next-unused sequence used by quotes and contract lookup.
     * The former last-issued convention could reuse an ID across request types;
     * retain it for review while keeping every observation owner distinct. */
#if 0
    uint32_t id = c->nextQuoteRequest + 1U;
#endif
    uint32_t id = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    UmiIbkrHistoricalStore *next = calloc(1, sizeof *next);
    if (!next)
        return UMI_STATUS_OUT_OF_MEMORY;
    char request[32], contract[32], duration[32];
    (void)snprintf(request, sizeof request, "%u", (unsigned)id);
    (void)snprintf(contract, sizeof contract, "%u", (unsigned)q->contract.contractId);
    (void)snprintf(duration, sizeof duration, "%u S", (unsigned)q->durationSeconds);
    /* conId and route identify an already resolved contract. No symbol guesses,
     * expired instruments, combination legs or continuous updates are inserted. */
    const char *fields[] = {"20",
                            request,
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
                            "0",
                            q->endUtc,
                            UmiIbkrHistoricalBarSetting(q->barSeconds),
                            duration,
                            q->regularHours ? "1" : "0",
                            UmiIbkrHistoricalDataSetting(q->dataKind),
                            "2",
                            "0",
                            ""};
    UmiStatus status = UmiIbkrQueueFields(c, fields, sizeof fields / sizeof fields[0]);
    if (status != UMI_STATUS_OK)
    {
        free(next);
        return status;
    }
    next->snapshot.requestId = id;
    next->snapshot.query = *q;
    next->snapshot.pending = true;
    next->snapshot.stale = true;
    next->snapshot.requestedAtMilliseconds = now;
    (void)snprintf(next->snapshot.message, sizeof next->snapshot.message,
                   "Waiting for a complete historical response.");
    /* Publish only after the entire request fits in the transport queue. */
    free(c->history);
    c->history = next;
    /* Reserve the following ID only after the complete request is queued.
     * The previous assignment is retained to explain the identity correction. */
#if 0
    c->nextQuoteRequest = id;
#endif
    c->nextQuoteRequest = id + 1U;
    c->lastNow = now;
    c->lastHistoryRequestAt = now;
    c->historyRequested = true;
    c->barRequested = true;
    c->lastBarRequestAt = now;
    *out = id;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrHistoricalCancel(UmiIbkrConnection *c, uint32_t request, uint64_t now)
{
    if (!c || !request || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->history || c->history->snapshot.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    if (c->snapshot.state != UMI_IBKR_READY || !c->history->snapshot.pending || TimedOut(c, now))
        return UMI_STATUS_INVALID_STATE;
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"25", "1", id};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 3U);
    if (status != UMI_STATUS_OK)
        return status;
    c->history->snapshot.pending = false;
    c->history->snapshot.cancelled = true;
    c->history->snapshot.stale = true;
    c->lastNow = now;
    (void)snprintf(c->history->snapshot.message, sizeof c->history->snapshot.message,
                   "Historical request cancelled locally; late responses will be ignored.");
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrHistoricalCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t age,
                                UmiIbkrHistoricalSnapshot *out)
{
    if (!c || !out || !request || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->history || c->history->snapshot.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    UmiIbkrHistoricalSnapshot s = c->history->snapshot;
    if (TimedOut(c, now))
    {
        s.pending = false;
        s.failed = true;
        (void)snprintf(s.message, sizeof s.message,
                       "Historical capture timed out; no complete series is available.");
    }
    s.stale = c->snapshot.state != UMI_IBKR_READY || !s.complete || s.failed || s.cancelled ||
              now - s.completedAtMilliseconds > age;
    *out = s;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrHistoricalBarCopy(const UmiIbkrConnection *c, uint32_t request, size_t index,
                                   UmiIbkrHistoricalBar *out)
{
    if (!c || !out || !request)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->history || c->history->snapshot.requestId != request || index >= c->history->snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    *out = c->history->bars[index];
    return UMI_STATUS_OK;
}
bool UmiIbkrHistoricalProviderMessage(UmiIbkrConnection *c, const char *id, int code, const char *message)
{
    uint64_t request;
    if (!c->history || !UmiIbkrUnsigned(id, &request) || request != c->history->snapshot.requestId)
        return false;
    if (c->history->snapshot.pending)
    {
        UmiIbkrHistoricalSnapshot *s = &c->history->snapshot;
        s->providerCode = code;
        s->pending = false;
        s->failed = true;
        s->stale = true;
        (void)snprintf(s->message, sizeof s->message, "%s", message);
    }
    return true;
}
