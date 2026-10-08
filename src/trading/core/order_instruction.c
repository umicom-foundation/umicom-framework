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
/* The earlier validator checked required positive prices but accepted unknown
 * order and time-in-force values. The explicit shared contract below prevents
 * unrecognized instructions from reaching routing. Keep the former rules here
 * for review of compatibility and the added rejection paths. */
#if 0
bool umi_trading_order_instruction_valid(const UmiTradingOrderInstruction *instruction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction==NULL||instruction->identity.client_order_id.value[0]=='\0'||instruction->instrument.instrument_id.value[0]=='\0'||instruction->quantity_lots<=0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(instruction->side!=UMI_SIDE_BUY&&instruction->side!=UMI_SIDE_SELL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((instruction->order_type==UMI_ORDER_LIMIT||instruction->order_type==UMI_ORDER_STOP_LIMIT)&&instruction->limit_ticks<=0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((instruction->order_type==UMI_ORDER_STOP||instruction->order_type==UMI_ORDER_STOP_LIMIT)&&instruction->stop_ticks<=0)return false;return true;}
#endif
bool umi_trading_order_instruction_valid(const UmiTradingOrderInstruction *instruction)
{
    if(instruction==NULL || instruction->quantity_lots<=0 ||
        instruction->identity.client_order_id.value[0]=='\0' ||
        instruction->instrument.instrument_id.value[0]=='\0' ||
        memchr(instruction->identity.client_order_id.value,'\0',
            sizeof instruction->identity.client_order_id.value)==NULL ||
        memchr(instruction->instrument.instrument_id.value,'\0',
            sizeof instruction->instrument.instrument_id.value)==NULL ||
        (instruction->side!=UMI_SIDE_BUY && instruction->side!=UMI_SIDE_SELL) ||
        instruction->limit_ticks<0 || instruction->stop_ticks<0) return false;
    /* Enumerate supported meanings rather than relying on numeric ranges.
     * A new order family must define its required prices before it is accepted. */
    switch(instruction->tif) {
        case UMI_TIF_DAY:case UMI_TIF_GTC:case UMI_TIF_IOC:case UMI_TIF_FOK:break;
        default:return false;
    }
    switch(instruction->order_type) {
        case UMI_ORDER_MARKET:return true;
        case UMI_ORDER_LIMIT:return instruction->limit_ticks>0;
        case UMI_ORDER_STOP:return instruction->stop_ticks>0;
        case UMI_ORDER_STOP_LIMIT:return instruction->limit_ticks>0 && instruction->stop_ticks>0;
        default:return false;
    }
}
