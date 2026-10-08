/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/realtime_decode.c
 * PURPOSE: Validate complete streaming bars before appending to the retained chronological window.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "observation_wire.h"
#include "market_bar_private.h"
#include <stdio.h>
#include <string.h>
static bool SameBar(const UmiIbkrRealtimeBar *a, const UmiIbkrRealtimeBar *b)
{
    /* Compare reported values, not struct padding or rounded floating-point
     * approximations. A conflicting repeat requires a new explicit capture. */
    return a->timeMilliseconds == b->timeMilliseconds && a->tradeCount == b->tradeCount &&
           !strcmp(a->open.reportedText, b->open.reportedText) &&
           !strcmp(a->high.reportedText, b->high.reportedText) &&
           !strcmp(a->low.reportedText, b->low.reportedText) &&
           !strcmp(a->close.reportedText, b->close.reportedText) &&
           !strcmp(a->volume.reportedText, b->volume.reportedText) &&
           !strcmp(a->weightedAveragePrice.reportedText, b->weightedAveragePrice.reportedText);
}
UmiStatus UmiIbkrRealtimeFrame(UmiIbkrConnection *c, const unsigned char *body, size_t length, uint64_t now)
{
    UmiIbkrObservationFields f = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length, 11U, &f);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t request, version;
    if (f.count < 3U || strcmp(f.values[0], "50") || !UmiIbkrUnsigned(f.values[1], &version) ||
        version < 1U || version > 3U || !UmiIbkrUnsigned(f.values[2], &request) || request > UINT32_MAX)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    UmiIbkrRealtimeStore *store = UmiIbkrRealtimeFind(c, (uint32_t)request);
    if (!store || !store->snapshot.active)
    {
        status = UmiIbkrObservationIgnore(c);
        goto done;
    }
    UmiIbkrRealtimeSnapshot *s = &store->snapshot;
    UmiIbkrRealtimeBar bar = {0};
    if (f.count != 11U || !UmiIbkrMarketBarRead(f.values + 3U, s->query.dataKind, &bar))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto failed;
    }
    bool gap = false;
    if (s->count)
    {
        size_t last = (store->first + s->count - 1U) % UMI_IBKR_REALTIME_BAR_LIMIT;
        const UmiIbkrRealtimeBar *previous = &store->bars[last];
        if (bar.timeMilliseconds <= previous->timeMilliseconds)
        {
            if (SameBar(&bar, previous))
            {
                if (s->duplicateBars == UINT64_MAX)
                {
                    status = UMI_STATUS_CAPACITY_EXCEEDED;
                    goto failed;
                }
                ++s->duplicateBars;
                goto done;
            }
            s->active = false;
            s->failed = true;
            s->stale = true;
            strcpy(s->message,
                   "A conflicting or out-of-order bar retired this series; cancel before resubscribing.");
            goto done;
        }
        if (bar.timeMilliseconds - previous->timeMilliseconds != 5000)
        {
            if (s->gapEvents == UINT64_MAX)
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                goto failed;
            }
            gap = true;
        }
    }
    if (s->receivedBars == UINT64_MAX)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto failed;
    }
    /* This ring discards only the oldest observation when full. A discarded
     * count and gap count travel with exports so a rolling view is never
     * mistaken for complete historical coverage. */
    if (gap)
        ++s->gapEvents;
    size_t at = (store->first + s->count) % UMI_IBKR_REALTIME_BAR_LIMIT;
    store->bars[at] = bar;
    if (s->count == UMI_IBKR_REALTIME_BAR_LIMIT)
    {
        store->first = (store->first + 1U) % UMI_IBKR_REALTIME_BAR_LIMIT;
        ++s->droppedBars;
    }
    else
        ++s->count;
    ++s->receivedBars;
    s->receivedAtMilliseconds = now;
    s->stale = false;
    strcpy(s->message, "Receiving five-second bars; volume uses the broker's reported units.");
    goto done;
failed:
    s->active = false;
    s->failed = true;
    s->stale = true;
    strcpy(s->message, "A streaming bar failed validation; retained rows were not replaced.");
done:
    UmiIbkrObservationFieldsClose(&f);
    return status;
}
