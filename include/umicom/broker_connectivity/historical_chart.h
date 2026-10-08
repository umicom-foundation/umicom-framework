/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/historical_chart.h
 * PURPOSE: Project retained broker bars through the reusable Framework candlestick renderer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_HISTORICAL_CHART_H
#define UMICOM_BROKER_CONNECTIVITY_HISTORICAL_CHART_H
#include "umicom/broker_connectivity/historical_bars.h"
#include "umicom/chart/plot.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Floating-point candles are presentation values only. Keep the original exact
 * decimal record for prices, quantities or calculations used by trading logic.
 * If volumeAvailable is false, candle.volume is an unused zero placeholder. */
    UmiStatus UmiIbkrHistoricalCandleCopy(const UmiIbkrConnection *connection, uint32_t requestId,
                                          size_t index, uint64_t nowMilliseconds,
                                          uint64_t maximumAgeMilliseconds, UmiChartCandle *outCandle,
                                          bool *outVolumeAvailable);
    /* Select a contiguous range to pan or zoom without another broker request.
 * The returned scene owns its commands and remains usable after disconnect.
 * On failure outScene is unchanged. Destroy successful scenes with
 * umi_chart_render_scene_destroy. Recheck snapshot freshness before displaying. */
    UmiStatus UmiIbkrHistoricalSceneCreate(const UmiIbkrConnection *connection, uint32_t requestId,
                                           size_t firstBar, size_t barCount, uint64_t nowMilliseconds,
                                           uint64_t maximumAgeMilliseconds, UmiChartRenderScene **outScene);
#ifdef __cplusplus
}
#endif
#endif
