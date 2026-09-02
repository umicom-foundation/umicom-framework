/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/tick_size_rule.h
 *
 * PURPOSE:
 *   Represent a price range and legal tick increment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_TICK_SIZE_RULE_H
#define UMICOM_TRADING_CORE_TICK_SIZE_RULE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading tick size rule data shared with callers of this public contract.
 */
typedef struct UmiTradingTickSizeRule { UmiTradingPriceTicks minimum_price; UmiTradingPriceTicks maximum_price; UmiTradingPriceTicks tick_size; } UmiTradingTickSizeRule;
/* Initialise and validate represent a price range and legal tick increment. */
UmiStatus umi_trading_tick_size_rule_init(UmiTradingTickSizeRule *value,UmiTradingPriceTicks minimum_price, UmiTradingPriceTicks maximum_price, UmiTradingPriceTicks tick_size);
/* Validate the invariant set for this trading record. */
bool umi_trading_tick_size_rule_valid(const UmiTradingTickSizeRule *value);
/* Calculate whether a representative price is tick aligned. */
bool umi_trading_tick_size_rule_aligned(const UmiTradingTickSizeRule *value);
#ifdef __cplusplus
}
#endif
#endif
