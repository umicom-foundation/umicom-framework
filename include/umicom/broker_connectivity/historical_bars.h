/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/historical_bars.h
 * PURPOSE: Capture finite intraday broker bars with explicit request identity and bounded ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_HISTORICAL_BARS_H
#define UMICOM_BROKER_CONNECTIVITY_HISTORICAL_BARS_H
#include "umicom/broker_connectivity/quotes.h"
#include "umicom/broker_connectivity/order_recovery.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_HISTORICAL_BAR_LIMIT 512U
#define UMI_IBKR_HISTORICAL_TIMEOUT_MS 60000U
#define UMI_IBKR_HISTORICAL_REQUEST_INTERVAL_MS 15000U
    /* Durations and bar sizes are seconds. This owner supports finite intraday
 * requests only; daily bars and live updates need different timestamp rules. */
    typedef enum UmiIbkrHistoricalDataKind
    {
        UMI_IBKR_HISTORY_TRADES = 1,
        UMI_IBKR_HISTORY_MIDPOINT = 2,
        UMI_IBKR_HISTORY_BID = 3,
        UMI_IBKR_HISTORY_ASK = 4
    } UmiIbkrHistoricalDataKind;
    typedef struct UmiIbkrHistoricalQuery
    {
        UmiIbkrQuoteContract contract;
        uint32_t durationSeconds, barSeconds;
        UmiIbkrHistoricalDataKind dataKind;
        bool regularHours;
        /* Empty means latest. Otherwise use YYYYMMDD HH:MM:SS UTC.
     * UTC makes the request independent of the user's machine time zone. */
        char endUtc[32];
    } UmiIbkrHistoricalQuery;
    typedef struct UmiIbkrHistoricalBar
    {
        int64_t timeMilliseconds;
        UmiIbkrOrderNumber open, high, low, close, volume, weightedAveragePrice;
        int32_t tradeCount;
        bool volumeAvailable, weightedAverageAvailable, tradeCountAvailable;
    } UmiIbkrHistoricalBar;
    typedef struct UmiIbkrHistoricalSnapshot
    {
        uint32_t requestId;
        UmiIbkrHistoricalQuery query;
        size_t count;
        bool pending, complete, failed, cancelled, stale;
        uint64_t requestedAtMilliseconds, completedAtMilliseconds;
        int providerCode;
        char startText[96], endText[96], message[256];
    } UmiIbkrHistoricalSnapshot;
    /* Validation leaves caller memory untouched. Extend the supported bar table
 * and its protocol tests together when adding another intraday interval. */
    UmiStatus UmiIbkrHistoricalQueryValidate(const UmiIbkrHistoricalQuery *query);
    UmiStatus UmiIbkrHistoricalRequest(UmiIbkrConnection *connection, const UmiIbkrHistoricalQuery *query,
                                       uint64_t nowMilliseconds, uint32_t *outRequestId);
    UmiStatus UmiIbkrHistoricalCancel(UmiIbkrConnection *connection, uint32_t requestId,
                                      uint64_t nowMilliseconds);
    /* Age measures when the capture arrived, not the market time of its last bar.
 * A completed response may contain an unfinished last bar. It is never live. */
    UmiStatus UmiIbkrHistoricalCopy(const UmiIbkrConnection *connection, uint32_t requestId,
                                    uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
                                    UmiIbkrHistoricalSnapshot *outSnapshot);
    /* Raw retained values remain readable after disconnect. Check the accompanying
 * snapshot before using them; exact=false prohibits exact decimal arithmetic. */
    UmiStatus UmiIbkrHistoricalBarCopy(const UmiIbkrConnection *connection, uint32_t requestId, size_t index,
                                       UmiIbkrHistoricalBar *outBar);
#ifdef __cplusplus
}
#endif
#endif
