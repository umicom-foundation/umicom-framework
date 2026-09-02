/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/volatility_interrupt.c
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
#include "umicom/trading/core/volatility_interrupt.h"

/*
 * Initialise trading volatility interrupt from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_volatility_interrupt_init(UmiTradingVolatilityInterrupt *interrupt,UmiTradingPriceTicks reference_ticks,uint32_t trigger_bps){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(interrupt==NULL||reference_ticks<=0||trigger_bps==0U||trigger_bps>10000U)return UMI_STATUS_INVALID_ARGUMENT;interrupt->reference_ticks=reference_ticks;interrupt->trigger_bps=trigger_bps;interrupt->auction_required=false;return UMI_STATUS_OK;}
/*
 * Provide the trading volatility interrupt evaluate operation used by this module and its
 * client applications.
 */
bool umi_trading_volatility_interrupt_evaluate(UmiTradingVolatilityInterrupt *interrupt,UmiTradingPriceTicks price_ticks){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(interrupt==NULL||price_ticks<=0)return false;int64_t diff=price_ticks-interrupt->reference_ticks;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(diff<0)diff=-diff;int64_t q=interrupt->reference_ticks/10000;int64_t r=interrupt->reference_ticks%10000;uint64_t allowance=(uint64_t)q*interrupt->trigger_bps+((uint64_t)r*interrupt->trigger_bps)/10000U;interrupt->auction_required=(uint64_t)diff>allowance;return interrupt->auction_required;}
