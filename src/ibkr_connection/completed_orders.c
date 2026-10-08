/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_orders.c
 * PURPOSE: Own completed-order capture independently from current orders and execution requests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static bool TimedOut(const UmiIbkrConnection *c, uint64_t now)
{
    return c->completed.snapshot.pending &&
           now - c->completed.snapshot.requestedAtMilliseconds >= c->options.timeoutMilliseconds;
}
void UmiIbkrCompletedExpire(UmiIbkrConnection *c, uint64_t now)
{
    if (!TimedOut(c, now))
        return;
    c->completed.snapshot.pending = false;
    c->completed.snapshot.failed = true;
    c->completed.snapshot.stale = true;
    (void)snprintf(c->completed.snapshot.message, sizeof c->completed.snapshot.message,
                   "Completed-order capture timed out. Reconnect; retained history is partial.");
}
UmiStatus UmiIbkrCompletedOrdersRequest(UmiIbkrConnection *c, bool apiOnly, uint64_t now)
{
    if (!c || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY || c->completed.snapshot.requested)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const char *fields[] = {"99", apiOnly ? "1" : "0"};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 2U);
    if (status != UMI_STATUS_OK)
        return status;
    UmiIbkrCompletedOrdersSnapshot *s = &c->completed.snapshot;
    s->requested = true;
    s->apiOnly = apiOnly;
    s->pending = true;
    s->stale = true;
    s->requestedAtMilliseconds = now;
    c->lastNow = now;
    (void)snprintf(s->message, sizeof s->message,
                   "Receiving the broker's available completed-order window. Wait for its end marker.");
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrCompletedOrdersCopy(const UmiIbkrConnection *c, uint64_t now, uint64_t age,
                                     UmiIbkrCompletedOrdersSnapshot *out)
{
    if (!c || !out || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrCompletedOrdersSnapshot copy = c->completed.snapshot;
    if (TimedOut(c, now))
    {
        copy.pending = false;
        copy.failed = true;
        (void)snprintf(copy.message, sizeof copy.message,
                       "Completed-order capture timed out. Reconnect; history is partial.");
    }
    copy.stale = c->snapshot.state != UMI_IBKR_READY || copy.failed || !copy.complete ||
                 now - copy.completedAtMilliseconds > age;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrCompletedOrderCopy(const UmiIbkrConnection *c, size_t index, uint64_t now, uint64_t age,
                                    UmiIbkrCompletedOrder *out)
{
    if (!c || !out || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= c->completed.snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    UmiIbkrCompletedOrder copy = c->completed.rows[index];
    copy.stale = c->snapshot.state != UMI_IBKR_READY || c->completed.snapshot.failed || TimedOut(c, now) ||
                 now - copy.receivedAtMilliseconds > age;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrCompletedOrderFieldCopy(const UmiIbkrConnection *c, size_t index, size_t field, char *buffer,
                                         size_t capacity, size_t *required)
{
    if (!c || !required || (!buffer && capacity))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= c->completed.snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    const unsigned char *bytes = c->completed.payloads[index];
    size_t length = c->completed.lengths[index], at = 0U;
    for (size_t i = 0U; at < length; ++i)
    {
        const unsigned char *end = memchr(bytes + at, 0, length - at);
        if (!end)
            return UMI_STATUS_PARSE_ERROR;
        size_t needed = (size_t)(end - (bytes + at)) + 1U;
        if (i == field)
        {
            if (capacity < needed)
            {
                *required = needed;
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            memcpy(buffer, bytes + at, needed);
            *required = needed;
            return UMI_STATUS_OK;
        }
        at += needed;
    }
    return UMI_STATUS_NOT_FOUND;
}
void UmiIbkrCompletedDestroy(UmiIbkrConnection *c)
{
    for (size_t i = 0U; i < UMI_IBKR_COMPLETED_ORDER_LIMIT; ++i)
    {
        free(c->completed.payloads[i]);
        c->completed.payloads[i] = NULL;
        c->completed.lengths[i] = 0U;
    }
}
