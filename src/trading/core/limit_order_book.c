/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/limit_order_book.c
 *
 * PURPOSE:
 *   Maintain bounded aggregated bid and ask levels with deterministic price ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/limit_order_book.h"

#include <string.h>
void umi_trading_limit_order_book_init(UmiTradingLimitOrderBook *book){if(book!=NULL)memset(book,0,sizeof *book);}
static void sort(UmiTradingOrderBookLevel *levels,size_t count,bool bid){for(size_t i=1;i<count;i++){UmiTradingOrderBookLevel key=levels[i];size_t j=i;while(j>0U&&((bid&&levels[j-1U].price_ticks<key.price_ticks)||(!bid&&levels[j-1U].price_ticks>key.price_ticks))){levels[j]=levels[j-1U];j--;}levels[j]=key;}}
UmiStatus umi_trading_limit_order_book_update(UmiTradingLimitOrderBook *book,UmiSide side,UmiTradingPriceTicks price,UmiTradingQuantityLots quantity,uint32_t order_count){if(book==NULL||price<=0||quantity<0||(side!=UMI_SIDE_BUY&&side!=UMI_SIDE_SELL))return UMI_STATUS_INVALID_ARGUMENT;UmiTradingOrderBookLevel *levels=side==UMI_SIDE_BUY?book->bids:book->asks;size_t *count=side==UMI_SIDE_BUY?&book->bid_count:&book->ask_count;size_t idx=*count;for(size_t i=0;i<*count;i++)if(levels[i].price_ticks==price){idx=i;break;}if(quantity==0){if(idx==*count)return UMI_STATUS_NOT_FOUND;for(size_t i=idx+1U;i<*count;i++)levels[i-1U]=levels[i];(*count)--;}else{if(idx==*count){if(*count>=UMI_TRADING_CORE_MAX_BOOK_LEVELS)return UMI_STATUS_CAPACITY_EXCEEDED;(*count)++;}levels[idx].price_ticks=price;levels[idx].quantity_lots=quantity;levels[idx].order_count=order_count;sort(levels,*count,side==UMI_SIDE_BUY);}book->revision++;return UMI_STATUS_OK;}
const UmiTradingOrderBookLevel *umi_trading_limit_order_book_best(const UmiTradingLimitOrderBook *book,UmiSide side){if(book==NULL)return NULL;if(side==UMI_SIDE_BUY)return book->bid_count>0U?&book->bids[0]:NULL;if(side==UMI_SIDE_SELL)return book->ask_count>0U?&book->asks[0]:NULL;return NULL;}
