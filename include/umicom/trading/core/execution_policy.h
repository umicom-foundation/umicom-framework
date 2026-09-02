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
#ifdef __cplusplus
}
#endif
#endif
