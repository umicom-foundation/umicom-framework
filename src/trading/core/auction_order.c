/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/auction_order.c
 *
 * PURPOSE:
 *   Represent eligible auction interest with side, price and timestamp priority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/auction_order.h"

#include <string.h>
/*
 * Initialise trading auction order from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_trading_auction_order_init(UmiTradingAuctionOrder *order,const char *order_id,UmiSide side,UmiTradingQuantityLots lots,UmiTradingPriceTicks limit_ticks,int64_t priority_time_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(order==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(order,0,sizeof *order);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_assign(&order->order_id,order_id)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;order->side=side;order->lots=lots;order->limit_ticks=limit_ticks;order->priority_time_ms=priority_time_ms;return umi_trading_auction_order_valid(order)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Check that trading auction order satisfies its contract before another service relies on
 * it.
 */
bool umi_trading_auction_order_valid(const UmiTradingAuctionOrder *order){return order!=NULL&&order->order_id.value[0]!='\0'&&(order->side==UMI_SIDE_BUY||order->side==UMI_SIDE_SELL)&&order->lots>0&&order->limit_ticks>0&&order->priority_time_ms>=0;}
