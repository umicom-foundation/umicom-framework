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
#ifdef __cplusplus
}
#endif
#endif
