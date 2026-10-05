/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/execution_policy.h
 *
 * PURPOSE:
 *   Define venue-count, participation and urgency bounds for execution strategies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_EXECUTION_POLICY_H
#define UMICOM_TRADING_CORE_EXECUTION_POLICY_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading execution policy data shared with callers of this public contract.
 */
typedef struct UmiTradingExecutionPolicy { uint32_t max_venues; uint32_t participation_bps; uint32_t urgency; } UmiTradingExecutionPolicy;
/* Initialise and validate define venue-count, participation and urgency bounds for execution strategies. */
UmiStatus umi_trading_execution_policy_init(UmiTradingExecutionPolicy *value,uint32_t max_venues, uint32_t participation_bps, uint32_t urgency);
/* Validate the invariant set for this trading record. */
bool umi_trading_execution_policy_valid(const UmiTradingExecutionPolicy *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_execution_policy_archive_encode(const UmiTradingExecutionPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_execution_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingExecutionPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
