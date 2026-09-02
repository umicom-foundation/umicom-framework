/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/auction_book.h
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
#ifndef UMICOM_TRADING_CORE_AUCTION_BOOK_H
#define UMICOM_TRADING_CORE_AUCTION_BOOK_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/auction_order.h"
/**
 * Represent the trading auction book data shared with callers of this public contract.
 */
typedef struct UmiTradingAuctionBook { UmiTradingAuctionOrder orders[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingAuctionBook;
/**
 * Initialise trading auction book from caller-provided values so later operations receive
 * a known state.
 */
void umi_trading_auction_book_init(UmiTradingAuctionBook *book);
/**
 * Add trading auction book only after its inputs and available capacity have been checked.
 */
UmiStatus umi_trading_auction_book_add(UmiTradingAuctionBook *book,const UmiTradingAuctionOrder *order);
/**
 * Provide the trading auction book executable operation used by this module and its client
 * applications.
 */
UmiTradingQuantityLots umi_trading_auction_book_executable(const UmiTradingAuctionBook *book,UmiTradingPriceTicks price);
#ifdef __cplusplus
}
#endif
#endif
