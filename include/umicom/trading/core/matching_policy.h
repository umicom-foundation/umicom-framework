/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/matching_policy.h
 *
 * PURPOSE:
 *   Define common exchange matching priorities and self-trade prevention behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MATCHING_POLICY_H
#define UMICOM_TRADING_CORE_MATCHING_POLICY_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading matching policy data shared with callers of this public contract.
 */
typedef struct UmiTradingMatchingPolicy { bool price_time_priority; bool prevent_self_trade; uint32_t max_matches_per_cycle; } UmiTradingMatchingPolicy;
/* Initialise and validate define common exchange matching priorities and self-trade prevention behaviour. */
UmiStatus umi_trading_matching_policy_init(UmiTradingMatchingPolicy *value,bool price_time_priority, bool prevent_self_trade, uint32_t max_matches_per_cycle);
/* Validate the invariant set for this trading record. */
bool umi_trading_matching_policy_valid(const UmiTradingMatchingPolicy *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_matching_policy_archive_encode(const UmiTradingMatchingPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_matching_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingMatchingPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
