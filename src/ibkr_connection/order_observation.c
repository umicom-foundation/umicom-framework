/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_observation.c
 * PURPOSE: Own one bounded order recovery and keep broker evidence separate from executions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool TimedOut(const UmiIbkrConnection *c, uint64_t now)
{
    return c->orders.snapshot.pending &&
           now - c->orders.snapshot.requestedAtMilliseconds >= c->options.timeoutMilliseconds;
}
static bool Unavailable(const UmiIbkrConnection *c, uint64_t now)
{
    return c->snapshot.state != UMI_IBKR_READY || c->orders.snapshot.failed || TimedOut(c, now);
}
void UmiIbkrOrdersExpire(UmiIbkrConnection *c, uint64_t now)
{
    if (TimedOut(c, now))
    {
        c->orders.snapshot.pending = false;
        c->orders.snapshot.failed = true;
        c->orders.snapshot.stale = true;
        (void)snprintf(
            c->orders.snapshot.message, sizeof c->orders.snapshot.message,
            "Open-order capture timed out. Reconnect for another snapshot; retained rows are partial.");
    }
}
UmiStatus UmiIbkrOrdersRequest(UmiIbkrConnection *c, UmiIbkrOrderScope scope, uint64_t now)
{
    if (!c || now < c->lastNow ||
        (scope != UMI_IBKR_ORDERS_THIS_CLIENT && scope != UMI_IBKR_ORDERS_ALL_CLIENTS))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY || c->orders.snapshot.requested)
        return UMI_STATUS_INVALID_STATE;
    const char *fields[] = {scope == UMI_IBKR_ORDERS_THIS_CLIENT ? "5" : "16", "1"};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 2U);
    if (status != UMI_STATUS_OK)
        return status;
    c->orders.snapshot.scope = scope;
    c->orders.snapshot.requested = true;
    c->orders.snapshot.pending = true;
    c->orders.snapshot.stale = true;
    c->orders.snapshot.requestedAtMilliseconds = now;
    c->lastNow = now;
    (void)snprintf(c->orders.snapshot.message, sizeof c->orders.snapshot.message,
                   "Receiving order summaries. Wait for the broker's end marker.");
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOrdersCopy(const UmiIbkrConnection *c, uint64_t now, uint64_t age,
                            UmiIbkrOrderRecoverySnapshot *out)
{
    if (!c || !out || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrOrderRecoverySnapshot copy = c->orders.snapshot;
    if (TimedOut(c, now))
    {
        copy.pending = false;
        copy.failed = true;
        (void)snprintf(copy.message, sizeof copy.message,
                       "Open-order capture timed out. Reconnect; retained rows are partial.");
    }
    copy.stale = Unavailable(c, now) || !copy.complete || now - copy.completedAtMilliseconds > age;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOrderCopy(const UmiIbkrConnection *c, size_t index, uint64_t now, uint64_t age,
                           UmiIbkrRecoveredOrder *out)
{
    if (!c || !out || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= c->orders.snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    UmiIbkrRecoveredOrder copy = c->orders.rows[index];
    copy.openStale = Unavailable(c, now) || !copy.hasOpenOrder || now - copy.openReceivedAtMilliseconds > age;
    copy.statusStale =
        Unavailable(c, now) || !copy.hasStatus || now - copy.statusReceivedAtMilliseconds > age;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOrderIdentityCopy(const UmiIbkrConnection *c, UmiIbkrOrderIdentitySnapshot *out)
{
    if (!c || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrOrderIdentitySnapshot copy = c->orders.identity;
    copy.stale = c->snapshot.state != UMI_IBKR_READY || !copy.brokerIdReceived;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOpenOrderFieldCopy(const UmiIbkrConnection *c, size_t index, size_t field, char *buffer,
                                    size_t capacity, size_t *required)
{
    if (!c || !required || (!buffer && capacity))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= c->orders.snapshot.count || !c->orders.rows[index].hasOpenOrder)
        return UMI_STATUS_NOT_FOUND;
    const unsigned char *data = c->orders.payloads[index];
    size_t length = c->orders.lengths[index], at = 0U;
    /* Walk only a previously bounded NUL-terminated frame. This also keeps
     * callers independent of allocations owned by the decoder. */
    for (size_t i = 0U; at < length; ++i)
    {
        const unsigned char *end = memchr(data + at, 0, length - at);
        if (!end)
            return UMI_STATUS_PARSE_ERROR;
        size_t needed = (size_t)(end - (data + at)) + 1U;
        if (i == field)
        {
            if (capacity < needed)
            {
                *required = needed;
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            memcpy(buffer, data + at, needed);
            *required = needed;
            return UMI_STATUS_OK;
        }
        at += needed;
    }
    return UMI_STATUS_NOT_FOUND;
}
void UmiIbkrOrdersDestroy(UmiIbkrConnection *c)
{
    for (size_t i = 0U; i < UMI_IBKR_ORDER_LIMIT; ++i)
    {
        free(c->orders.payloads[i]);
        c->orders.payloads[i] = NULL;
        c->orders.lengths[i] = 0U;
    }
}
