/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_instruction.c
 *
 * PURPOSE:
 *   Normalise an existing order request into integer price-tick and lot instructions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_instruction.h"

#include <string.h>
/* Initialise a normalised order instruction. */
UmiStatus umi_trading_order_instruction_init(UmiTradingOrderInstruction *instruction,const UmiTradingOrderIdentity *identity,const UmiInstrument *instrument,UmiSide side,UmiOrderType type,UmiTimeInForce tif,UmiTradingQuantityLots quantity_lots,UmiTradingPriceTicks limit_ticks,UmiTradingPriceTicks stop_ticks){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction==NULL||identity==NULL||instrument==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(instruction,0,sizeof *instruction);instruction->identity=*identity;instruction->instrument=*instrument;instruction->side=side;instruction->order_type=type;instruction->tif=tif;instruction->quantity_lots=quantity_lots;instruction->limit_ticks=limit_ticks;instruction->stop_ticks=stop_ticks;return umi_trading_order_instruction_valid(instruction)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
/* Validate quantity and price requirements for the selected order type. */
bool umi_trading_order_instruction_valid(const UmiTradingOrderInstruction *instruction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction==NULL||instruction->identity.client_order_id.value[0]=='\0'||instruction->instrument.instrument_id.value[0]=='\0'||instruction->quantity_lots<=0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction->side!=UMI_SIDE_BUY&&instruction->side!=UMI_SIDE_SELL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((instruction->order_type==UMI_ORDER_LIMIT||instruction->order_type==UMI_ORDER_STOP_LIMIT)&&instruction->limit_ticks<=0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((instruction->order_type==UMI_ORDER_STOP||instruction->order_type==UMI_ORDER_STOP_LIMIT)&&instruction->stop_ticks<=0)return false;return true;}
