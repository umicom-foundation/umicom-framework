/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_instruction.h
 *
 * PURPOSE:
 *   Normalise an existing order request into integer price-tick and lot instructions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_INSTRUCTION_H
#define UMICOM_TRADING_CORE_ORDER_INSTRUCTION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/order_identity.h"
typedef struct UmiTradingOrderInstruction { UmiTradingOrderIdentity identity; UmiInstrument instrument; UmiSide side; UmiOrderType order_type; UmiTimeInForce tif; UmiTradingQuantityLots quantity_lots; UmiTradingPriceTicks limit_ticks; UmiTradingPriceTicks stop_ticks; bool reduce_only; } UmiTradingOrderInstruction;
/* Initialise a normalised order instruction. */
UmiStatus umi_trading_order_instruction_init(UmiTradingOrderInstruction *instruction,const UmiTradingOrderIdentity *identity,const UmiInstrument *instrument,UmiSide side,UmiOrderType type,UmiTimeInForce tif,UmiTradingQuantityLots quantity_lots,UmiTradingPriceTicks limit_ticks,UmiTradingPriceTicks stop_ticks);
/* Validate quantity and price requirements for the selected order type. */
bool umi_trading_order_instruction_valid(const UmiTradingOrderInstruction *instruction);
#ifdef __cplusplus
}
#endif
#endif
