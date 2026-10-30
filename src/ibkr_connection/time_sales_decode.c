/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/time_sales_decode.c
 *
 * PURPOSE:
 *   Validate complete tick events before appending lossless observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "observation_wire.h"
#include "order_numbers.h"
#include <string.h>

static bool KindMatches(UmiIbkrTimeSalesKind requested, uint64_t received)
{
    if (requested == UMI_IBKR_TICKS_ALL_LAST)
        return received == 1U || received == 2U;
    return received == (uint64_t)requested;
}
static bool ReadEvent(char **v, size_t count, UmiIbkrTimeSalesKind requested,
                       uint64_t now, UmiIbkrTimeSalesEvent *out)
{
    uint64_t kind, seconds, flags = 0;
    if (count < 5U || !UmiIbkrUnsigned(v[2], &kind) || !KindMatches(requested, kind) ||
        !UmiIbkrUnsigned(v[3], &seconds) || seconds > UINT64_C(253402300799))
        return false;
    UmiIbkrTimeSalesEvent event = {0};
    event.kind = (UmiIbkrTimeSalesKind)kind;
    event.epochSeconds = seconds;
    event.receivedAtMilliseconds = now;
    if (kind == 1U || kind == 2U) {
        if (count != 9U ||
            !UmiIbkrOrderNumberRead(v[4], false, false, &event.price) ||
            !UmiIbkrOrderNumberRead(v[5], false, true, &event.size) ||
            !UmiIbkrUnsigned(v[6], &flags) || flags > 3U ||
            !UmiIbkrText(v[7], sizeof event.exchange, true) ||
            !UmiIbkrText(v[8], sizeof event.specialConditions, true))
            return false;
        strcpy(event.exchange, v[7]);
        strcpy(event.specialConditions, v[8]);
    } else if (kind == 3U) {
        if (count != 9U ||
            !UmiIbkrOrderNumberRead(v[4], false, false, &event.bidPrice) ||
            !UmiIbkrOrderNumberRead(v[5], false, false, &event.askPrice) ||
            !UmiIbkrOrderNumberRead(v[6], false, true, &event.bidSize) ||
            !UmiIbkrOrderNumberRead(v[7], false, true, &event.askSize) ||
            !UmiIbkrUnsigned(v[8], &flags) || flags > 3U)
            return false;
    } else if (kind == 4U) {
        if (count != 5U || !UmiIbkrOrderNumberRead(v[4], false, false, &event.price))
            return false;
    } else {
        return false;
    }
    event.flags = (unsigned)flags;
    *out = event;
    return true;
}
UmiStatus UmiIbkrTimeSalesFrame(UmiIbkrConnection *c, const unsigned char *body,
                               size_t length, uint64_t now)
{
    UmiIbkrObservationFields fields = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length, 9U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t request;
    if (fields.count < 3U || strcmp(fields.values[0], "99") ||
        !UmiIbkrUnsigned(fields.values[1], &request) || request > UINT32_MAX) {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    UmiIbkrTimeSalesStore *store = UmiIbkrTimeSalesFind(c, (uint32_t)request);
    if (!store || !store->snapshot.active) {
        status = UmiIbkrObservationIgnore(c);
        goto done;
    }
    UmiIbkrTimeSalesSnapshot *s = &store->snapshot;
    UmiIbkrTimeSalesEvent event;
    if (!ReadEvent(fields.values, fields.count, s->query.kind, now, &event)) {
        status = UMI_STATUS_PARSE_ERROR;
        goto failed;
    }
    if (s->receivedEvents == UINT64_MAX ||
        (s->count == UMI_IBKR_TIME_SALES_EVENT_LIMIT && s->droppedEvents == UINT64_MAX)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto failed;
    }
    /* Commit only after validating every field. Keep receipt order and equal
     * timestamps: different trades can share the same second and price. */
    event.sequence = s->receivedEvents + 1U;
    size_t at = (store->first + s->count) % UMI_IBKR_TIME_SALES_EVENT_LIMIT;
    store->events[at] = event;
    if (s->count == UMI_IBKR_TIME_SALES_EVENT_LIMIT) {
        store->first = (store->first + 1U) % UMI_IBKR_TIME_SALES_EVENT_LIMIT;
        ++s->droppedEvents;
    } else {
        ++s->count;
    }
    ++s->receivedEvents;
    s->receivedAtMilliseconds = now;
    s->stale = false;
    strcpy(s->message, "Receiving tick observations; receipt age does not prove market freshness.");
    goto done;
failed:
    s->active = false;
    s->failed = true;
    s->stale = true;
    strcpy(s->message, "Tick validation failed; retained events were not replaced.");
done:
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
