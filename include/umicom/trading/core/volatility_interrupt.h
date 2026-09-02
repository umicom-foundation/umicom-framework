/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/volatility_interrupt.h
 *
 * PURPOSE:
 *   Track short-horizon reference-price deviations and recommend auction interruption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_VOLATILITY_INTERRUPT_H
#define UMICOM_TRADING_CORE_VOLATILITY_INTERRUPT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading volatility interrupt data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingVolatilityInterrupt { UmiTradingPriceTicks reference_ticks; uint32_t trigger_bps; bool auction_required; } UmiTradingVolatilityInterrupt;
/**
 * Initialise trading volatility interrupt from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_volatility_interrupt_init(UmiTradingVolatilityInterrupt *interrupt,UmiTradingPriceTicks reference_ticks,uint32_t trigger_bps);
/**
 * Provide the trading volatility interrupt evaluate operation used by this module and its
 * client applications.
 */
bool umi_trading_volatility_interrupt_evaluate(UmiTradingVolatilityInterrupt *interrupt,UmiTradingPriceTicks price_ticks);
#ifdef __cplusplus
}
#endif
#endif
