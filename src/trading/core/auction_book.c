/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/auction_book.c
 *
 * PURPOSE:
 *   Maintain bounded auction buy and sell interest before uncrossing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/auction_book.h"

#include <string.h>
/*
 * Initialise trading auction book from caller-provided values so later operations receive
 * a known state.
 */
void umi_trading_auction_book_init(UmiTradingAuctionBook *book){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book!=NULL)memset(book,0,sizeof *book);}
/* Add trading auction book only after its inputs and available capacity have been checked. */
UmiStatus umi_trading_auction_book_add(UmiTradingAuctionBook *book,const UmiTradingAuctionOrder *order){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book==NULL||!umi_trading_auction_order_valid(order))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<book->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&book->orders[i].order_id,&order->order_id))return UMI_STATUS_ALREADY_EXISTS;book->orders[book->count++]=*order;return UMI_STATUS_OK;}
/*
 * Provide the trading auction book executable operation used by this module and its client
 * applications.
 */
UmiTradingQuantityLots umi_trading_auction_book_executable(const UmiTradingAuctionBook *book,UmiTradingPriceTicks price){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book==NULL||price<=0)return 0;int64_t buy=0,sell=0;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<book->count;i++){const UmiTradingAuctionOrder *o=&book->orders[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o->side==UMI_SIDE_BUY&&o->limit_ticks>=price)(void)umi_trading_core_add_i64(buy,o->lots,&buy);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o->side==UMI_SIDE_SELL&&o->limit_ticks<=price)(void)umi_trading_core_add_i64(sell,o->lots,&sell);}return buy<sell?buy:sell;}
