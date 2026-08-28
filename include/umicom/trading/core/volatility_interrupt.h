/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/volatility_interrupt.h
 *
 * PURPOSE:
 *   Track short-horizon reference-price deviations and recommend auction interruption.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_VOLATILITY_INTERRUPT_H
#define UMICOM_TRADING_CORE_VOLATILITY_INTERRUPT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingVolatilityInterrupt { UmiTradingPriceTicks reference_ticks; uint32_t trigger_bps; bool auction_required; } UmiTradingVolatilityInterrupt;
UmiStatus umi_trading_volatility_interrupt_init(UmiTradingVolatilityInterrupt *interrupt,UmiTradingPriceTicks reference_ticks,uint32_t trigger_bps);
bool umi_trading_volatility_interrupt_evaluate(UmiTradingVolatilityInterrupt *interrupt,UmiTradingPriceTicks price_ticks);
#ifdef __cplusplus
}
#endif
#endif
