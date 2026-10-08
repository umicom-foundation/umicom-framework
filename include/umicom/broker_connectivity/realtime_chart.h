/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/realtime_chart.h
 * PURPOSE: Project fresh streaming bars into owned portable candlestick scenes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_REALTIME_CHART_H
#define UMICOM_BROKER_CONNECTIVITY_REALTIME_CHART_H
#include "umicom/broker_connectivity/realtime_bars.h"
#include "umicom/broker_connectivity/historical_chart.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Link Umicom::ibkr_historical_chart, the existing optional chart owner.
 * Failure leaves the output untouched. An owned scene is a frozen display:
 * compare receivedBars and freshness before reusing it on a later UI frame. */
    UmiStatus UmiIbkrRealtimeCandleCopy(const UmiIbkrConnection *, uint32_t requestId, size_t index,
                                        uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
                                        UmiChartCandle *outCandle, bool *outVolumeAvailable);
    UmiStatus UmiIbkrRealtimeSceneCreate(const UmiIbkrConnection *, uint32_t requestId, size_t first,
                                         size_t count, uint64_t nowMilliseconds,
                                         uint64_t maximumAgeMilliseconds, UmiChartRenderScene **outScene);
#ifdef __cplusplus
}
#endif
#endif
