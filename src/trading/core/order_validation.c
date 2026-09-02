/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_validation.c
 *
 * PURPOSE:
 *   Apply shared lot, tick and price-band checks before an order reaches routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_validation.h"

#include <string.h>
/* Set a bounded validation decision reason. */
static UmiTradingCoreDecision decision(bool allowed,const char *reason){UmiTradingCoreDecision d={0};d.allowed=allowed;(void)umi_trading_core_copy_text(d.reason,sizeof d.reason,reason);return d;}
/* Validate an order against reusable venue microstructure rules. */
UmiTradingCoreDecision umi_trading_order_validation_check(const UmiTradingOrderInstruction *instruction,const UmiTradingTickSizeRule *tick_rule,const UmiTradingLotSizeRule *lot_rule,const UmiTradingPriceBand *band){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_trading_order_instruction_valid(instruction)||!umi_trading_tick_size_rule_valid(tick_rule)||!umi_trading_lot_size_rule_valid(lot_rule))return decision(false,"invalid-input");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction->quantity_lots<lot_rule->minimum_lots||instruction->quantity_lots>lot_rule->maximum_lots||((instruction->quantity_lots-lot_rule->minimum_lots)%lot_rule->step_lots)!=0)return decision(false,"invalid-lot");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction->limit_ticks>0&&(instruction->limit_ticks%tick_rule->tick_size)!=0)return decision(false,"invalid-tick");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction->limit_ticks>0&&band!=NULL&&!umi_trading_price_band_contains(band,instruction->limit_ticks))return decision(false,"outside-band");return decision(true,"accepted");}
