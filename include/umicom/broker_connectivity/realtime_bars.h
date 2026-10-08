/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/realtime_bars.h
 * PURPOSE: Own bounded five-second market-data subscriptions independently of application windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_REALTIME_BARS_H
#define UMICOM_BROKER_CONNECTIVITY_REALTIME_BARS_H
#include "umicom/broker_connectivity/historical_bars.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_REALTIME_STREAM_LIMIT 4U
#define UMI_IBKR_REALTIME_BAR_LIMIT 512U
#define UMI_IBKR_BAR_REQUEST_INTERVAL_MS 15000U
    /* Reuse the exact same decimal record for finite and streaming bars. Availability
 * flags describe provider fields; volume is retained in provider units, not lots
 * or shares inferred from a symbol. */
    typedef UmiIbkrHistoricalBar UmiIbkrRealtimeBar;
    typedef struct UmiIbkrRealtimeQuery
    {
        UmiIbkrQuoteContract contract;
        UmiIbkrHistoricalDataKind dataKind;
        bool regularHours;
    } UmiIbkrRealtimeQuery;
    typedef struct UmiIbkrRealtimeSnapshot
    {
        uint32_t requestId;
        UmiIbkrRealtimeQuery query;
        size_t count;
        bool active, needsCancel, failed, cancelled, stale;
        uint64_t requestedAtMilliseconds, receivedAtMilliseconds;
        uint64_t receivedBars, droppedBars, gapEvents, duplicateBars;
        int providerCode;
        char message[256];
    } UmiIbkrRealtimeSnapshot;
    /* Requests use resolved contract IDs and routes, with five-second bars only.
 * A conservative shared 15-second interval also covers historical requests.
 * Other connections and applications still consume the account's broker limits.
 * As with the connection API, serialize calls on one owner thread. */
    UmiStatus UmiIbkrRealtimeQueryValidate(const UmiIbkrRealtimeQuery *query);
    UmiStatus UmiIbkrRealtimeRequest(UmiIbkrConnection *connection, const UmiIbkrRealtimeQuery *query,
                                     uint64_t nowMilliseconds, uint32_t *outRequestId);
    UmiStatus UmiIbkrRealtimeCancel(UmiIbkrConnection *connection, uint32_t requestId,
                                    uint64_t nowMilliseconds);
    /* Freshness measures time since a newly accepted bar arrived. Duplicate packets
 * cannot keep an idle series fresh. A broker error retires the series; cancel it
 * explicitly before using its slot for another subscription. */
    UmiStatus UmiIbkrRealtimeCopy(const UmiIbkrConnection *connection, uint32_t requestId,
                                  uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
                                  UmiIbkrRealtimeSnapshot *outSnapshot);
    /* Index zero is the oldest retained bar. Records remain readable after cancel
 * or disconnect, but their accompanying snapshot must always travel with them. */
    UmiStatus UmiIbkrRealtimeBarCopy(const UmiIbkrConnection *connection, uint32_t requestId, size_t index,
                                     UmiIbkrRealtimeBar *outBar);
#ifdef __cplusplus
}
#endif
#endif
