/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/realtime_chart.c
 * PURPOSE: Require fresh streaming observations before building an independent chart scene.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/realtime_chart.h"
#include "bar_chart_private.h"
#include <stdlib.h>
UmiStatus UmiIbkrRealtimeCandleCopy(const UmiIbkrConnection *c, uint32_t request, size_t index, uint64_t now,
                                    uint64_t age, UmiChartCandle *out, bool *volume)
{
    if (!out || !volume)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrRealtimeSnapshot s;
    UmiStatus status = UmiIbkrRealtimeCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    if (s.stale)
        return UMI_STATUS_INVALID_STATE;
    UmiIbkrRealtimeBar bar;
    status = UmiIbkrRealtimeBarCopy(c, request, index, &bar);
    if (status != UMI_STATUS_OK)
        return status;
    return UmiIbkrMarketBarCandle(&bar, out, volume);
}
/* The earlier adapter dropped volume availability before scene creation.
 * Retain it for review; the replacement carries those flags through projection. */
#if 0
UmiStatus UmiIbkrRealtimeSceneCreate(const UmiIbkrConnection *c, uint32_t request, size_t first, size_t count,
                                     uint64_t now, uint64_t age, UmiChartRenderScene **out)
{
    if (!out || !count || count > UMI_IBKR_REALTIME_BAR_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrRealtimeSnapshot s;
    UmiStatus status = UmiIbkrRealtimeCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    if (s.stale)
        return UMI_STATUS_INVALID_STATE;
    if (first > s.count || count > s.count - first)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A temporary contiguous view keeps ring-buffer details out of the renderer.
     * Time gaps retain their real positions; no synthetic candles are inserted. */
    UmiChartCandle *candles = calloc(count, sizeof *candles);
    if (!candles)
        return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0; i < count; ++i)
    {
        bool volume;
        status = UmiIbkrRealtimeCandleCopy(c, request, first + i, now, age, &candles[i], &volume);
        if (status != UMI_STATUS_OK)
        {
            free(candles);
            return status;
        }
    }
    status = UmiIbkrMarketBarScene(candles, count,
                                   "Five-second bars | rolling window | provider volume units", out);
    free(candles);
    return status;
}
#endif

UmiStatus UmiIbkrRealtimeSceneCreate(const UmiIbkrConnection *c, uint32_t request, size_t first,
    size_t count, uint64_t now, uint64_t age, UmiChartRenderScene **out)
{
    if (out == NULL || count == 0U || count > UMI_IBKR_REALTIME_BAR_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrRealtimeSnapshot s;
    UmiStatus status = UmiIbkrRealtimeCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    if (s.stale)
        return UMI_STATUS_INVALID_STATE;
    if (first > s.count || count > s.count - first)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartCandle *candles = calloc(count, sizeof *candles);
    bool *available = calloc(count, sizeof *available);
    if (candles == NULL || available == NULL) {
        free(candles);
        free(available);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Retain a separate observation flag. A zero placeholder for missing volume
     * must not become an observed zero when this view reaches the renderer. */
    for (size_t i = 0U; i < count; ++i) {
        status = UmiIbkrRealtimeCandleCopy(c, request, first + i, now, age,
                                          &candles[i], &available[i]);
        if (status != UMI_STATUS_OK)
            break;
    }
    if (status == UMI_STATUS_OK)
        status = UmiIbkrMarketBarSceneObserved(candles, available, count,
            "Five-second bars | rolling window | provider volume units", out);
    free(available);
    free(candles);
    return status;
}
