/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/market_depth.h
 * PURPOSE: Own bounded bid and ask ladders from broker market-depth updates.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_MARKET_DEPTH_H
#define UMICOM_BROKER_CONNECTIVITY_MARKET_DEPTH_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/finance/decimal.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_DEPTH_STREAM_LIMIT 3U
#define UMI_IBKR_DEPTH_ROW_LIMIT 50U
    typedef struct UmiIbkrDepthSelection
    {
        uint32_t contractId;
        char symbol[64], securityType[16], expiry[32], strike[96], right[8], multiplier[32];
        char exchange[64], primaryExchange[64], currency[16], localSymbol[64], tradingClass[64];
        uint32_t rows;
        bool smartDepth;
    } UmiIbkrDepthSelection;
    typedef struct UmiIbkrDepthRow
    {
        UmiDecimal price, size;
        char marketMaker[64];
        uint64_t receivedAtMilliseconds;
        bool hasMarketMaker, stale;
    } UmiIbkrDepthRow;
    typedef struct UmiIbkrDepthSnapshot
    {
        uint32_t requestId;
        UmiIbkrDepthSelection selection;
        size_t bidCount, askCount;
        UmiIbkrDepthRow bids[UMI_IBKR_DEPTH_ROW_LIMIT], asks[UMI_IBKR_DEPTH_ROW_LIMIT];
        bool subscribed, received, failed, stale;
        uint64_t requestedAtMilliseconds, receivedAtMilliseconds, revision, resetCount;
        int providerCode;
        char message[256];
    } UmiIbkrDepthSnapshot;
    /* Use a confirmed positive contract ID and an explicit exchange. Optional
 * descriptors are forwarded as supplied; an empty strike is encoded as zero.
 * SMART aggregation requires exchange "SMART" and smartDepth=true. Direct
 * depth requires a named exchange and smartDepth=false. Combo depth is refused.
 * Internal-use provider options are intentionally sent empty.
 * The local limit does not grant a broker data entitlement or subscription slot.
 * No order is submitted; unchanged output on failure. The connection owns copies
 * of every input string and should be pumped by the same owner thread. */
    UmiStatus UmiIbkrDepthSubscribe(UmiIbkrConnection *connection, const UmiIbkrDepthSelection *selection,
                                    uint64_t nowMilliseconds, uint32_t *outRequestId);
    UmiStatus UmiIbkrDepthCancel(UmiIbkrConnection *connection, uint32_t requestId);
    /* Each side is kept in the provider's row order, not locally sorted. Insert,
 * update and delete operations are positional. There is no complete-book marker.
 * A provider reset empties both sides before accepting subsequent inserts.
 * Ages describe receipt time; they are not exchange timestamps or a fill promise.
 * Out must not overlap connection-owned storage. */
    UmiStatus UmiIbkrDepthCopy(const UmiIbkrConnection *connection, uint32_t requestId,
                               uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
                               UmiIbkrDepthSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif
