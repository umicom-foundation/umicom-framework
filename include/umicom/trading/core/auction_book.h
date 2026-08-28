/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/auction_book.h
 *
 * PURPOSE:
 *   Maintain bounded auction buy and sell interest before uncrossing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_AUCTION_BOOK_H
#define UMICOM_TRADING_CORE_AUCTION_BOOK_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/auction_order.h"
typedef struct UmiTradingAuctionBook { UmiTradingAuctionOrder orders[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingAuctionBook;
void umi_trading_auction_book_init(UmiTradingAuctionBook *book);
UmiStatus umi_trading_auction_book_add(UmiTradingAuctionBook *book,const UmiTradingAuctionOrder *order);
UmiTradingQuantityLots umi_trading_auction_book_executable(const UmiTradingAuctionBook *book,UmiTradingPriceTicks price);
#ifdef __cplusplus
}
#endif
#endif
