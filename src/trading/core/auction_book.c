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
void umi_trading_auction_book_init(UmiTradingAuctionBook *book){if(book!=NULL)memset(book,0,sizeof *book);}
UmiStatus umi_trading_auction_book_add(UmiTradingAuctionBook *book,const UmiTradingAuctionOrder *order){if(book==NULL||!umi_trading_auction_order_valid(order))return UMI_STATUS_INVALID_ARGUMENT;if(book->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t i=0;i<book->count;i++)if(umi_trading_core_id_equal(&book->orders[i].order_id,&order->order_id))return UMI_STATUS_ALREADY_EXISTS;book->orders[book->count++]=*order;return UMI_STATUS_OK;}
UmiTradingQuantityLots umi_trading_auction_book_executable(const UmiTradingAuctionBook *book,UmiTradingPriceTicks price){if(book==NULL||price<=0)return 0;int64_t buy=0,sell=0;for(size_t i=0;i<book->count;i++){const UmiTradingAuctionOrder *o=&book->orders[i];if(o->side==UMI_SIDE_BUY&&o->limit_ticks>=price)(void)umi_trading_core_add_i64(buy,o->lots,&buy);if(o->side==UMI_SIDE_SELL&&o->limit_ticks<=price)(void)umi_trading_core_add_i64(sell,o->lots,&sell);}return buy<sell?buy:sell;}
