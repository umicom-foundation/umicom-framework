/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/auction_match.c
 *
 * PURPOSE:
 *   Choose a deterministic auction clearing price by maximum executable quantity then minimum imbalance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/auction_match.h"

#include <string.h>
/*
 * Provide the trading auction match compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_auction_match_compute(const UmiTradingAuctionBook *book,UmiTradingAuctionMatch *out_match){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book==NULL||out_match==NULL||book->count==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(out_match,0,sizeof *out_match);/* Visit each bounded item once so every record receives the same rule. */ for(size_t c=0;c<book->count;c++){UmiTradingPriceTicks p=book->orders[c].limit_ticks;int64_t buy=0,sell=0;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<book->count;i++){const UmiTradingAuctionOrder *o=&book->orders[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o->side==UMI_SIDE_BUY&&o->limit_ticks>=p)(void)umi_trading_core_add_i64(buy,o->lots,&buy);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o->side==UMI_SIDE_SELL&&o->limit_ticks<=p)(void)umi_trading_core_add_i64(sell,o->lots,&sell);}int64_t executable=buy<sell?buy:sell;int64_t imbalance=buy-sell;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(imbalance<0)imbalance=-imbalance;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!out_match->matched||executable>out_match->executable_lots||(executable==out_match->executable_lots&&imbalance<out_match->imbalance_lots)||(executable==out_match->executable_lots&&imbalance==out_match->imbalance_lots&&p<out_match->clearing_ticks)){out_match->clearing_ticks=p;out_match->executable_lots=executable;out_match->imbalance_lots=imbalance;out_match->matched=executable>0;}}return out_match->matched?UMI_STATUS_OK:UMI_STATUS_NOT_FOUND;}
