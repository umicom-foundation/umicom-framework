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
