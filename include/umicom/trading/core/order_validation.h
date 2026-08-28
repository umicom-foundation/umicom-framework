/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_validation.h
 *
 * PURPOSE:
 *   Apply shared lot, tick and price-band checks before an order reaches routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_VALIDATION_H
#define UMICOM_TRADING_CORE_ORDER_VALIDATION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/order_instruction.h"
#include "umicom/trading/core/tick_size_rule.h"
#include "umicom/trading/core/lot_size_rule.h"
#include "umicom/trading/core/price_band.h"
/* Validate an order against reusable venue microstructure rules. */
UmiTradingCoreDecision umi_trading_order_validation_check(const UmiTradingOrderInstruction *instruction,const UmiTradingTickSizeRule *tick_rule,const UmiTradingLotSizeRule *lot_rule,const UmiTradingPriceBand *band);
#ifdef __cplusplus
}
#endif
#endif
