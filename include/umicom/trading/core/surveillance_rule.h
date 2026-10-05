/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/surveillance_rule.h
 *
 * PURPOSE:
 *   Define reusable market-surveillance thresholds and alert severity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_SURVEILLANCE_RULE_H
#define UMICOM_TRADING_CORE_SURVEILLANCE_RULE_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading surveillance rule data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingSurveillanceRule { uint32_t threshold; uint32_t window_seconds; UmiTradingCoreSeverity severity; } UmiTradingSurveillanceRule;
/* Initialise and validate define reusable market-surveillance thresholds and alert severity. */
UmiStatus umi_trading_surveillance_rule_init(UmiTradingSurveillanceRule *value,uint32_t threshold, uint32_t window_seconds, UmiTradingCoreSeverity severity);
/* Validate the invariant set for this trading record. */
bool umi_trading_surveillance_rule_valid(const UmiTradingSurveillanceRule *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_surveillance_rule_archive_encode(const UmiTradingSurveillanceRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_surveillance_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingSurveillanceRule *value);

#ifdef __cplusplus
}
#endif
#endif
