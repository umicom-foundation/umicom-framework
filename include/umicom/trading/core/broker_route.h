/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/broker_route.h
 *
 * PURPOSE:
 *   Describe a candidate broker/venue route with cost and latency scores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BROKER_ROUTE_H
#define UMICOM_TRADING_CORE_BROKER_ROUTE_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading broker route data shared with callers of this public contract.
 */
typedef struct UmiTradingBrokerRoute { UmiFinancialId route_id; UmiFinancialId venue_id; uint32_t cost_bps; uint32_t latency_score; bool enabled; } UmiTradingBrokerRoute;
/* Initialise and validate describe a candidate broker/venue route with cost and latency scores. */
UmiStatus umi_trading_broker_route_init(UmiTradingBrokerRoute *value,const UmiFinancialId * route_id, const UmiFinancialId * venue_id, uint32_t cost_bps, uint32_t latency_score, bool enabled);
/* Validate the invariant set for this trading record. */
bool umi_trading_broker_route_valid(const UmiTradingBrokerRoute *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_broker_route_archive_encode(const UmiTradingBrokerRoute *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_broker_route_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingBrokerRoute *value);

#ifdef __cplusplus
}
#endif
#endif
