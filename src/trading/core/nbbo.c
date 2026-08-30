/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/nbbo.c
 *
 * PURPOSE:
 *   Calculate a national-best-style bid and offer across bounded venue quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/nbbo.h"

#include <string.h>
UmiStatus umi_trading_nbbo_compute(const UmiTradingConsolidatedQuote *quotes,size_t count,UmiTradingNbbo *out_nbbo){if(quotes==NULL||out_nbbo==NULL||count==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(out_nbbo,0,sizeof *out_nbbo);for(size_t i=0;i<count;i++){const UmiTradingConsolidatedQuote *q=&quotes[i];if(!umi_trading_consolidated_quote_valid(q))return UMI_STATUS_INVALID_ARGUMENT;if(!out_nbbo->valid||q->bid_ticks>out_nbbo->best_bid_ticks){out_nbbo->best_bid_ticks=q->bid_ticks;out_nbbo->best_bid_lots=q->bid_lots;out_nbbo->bid_venue_id=q->venue_id;}if(!out_nbbo->valid||q->ask_ticks<out_nbbo->best_ask_ticks){out_nbbo->best_ask_ticks=q->ask_ticks;out_nbbo->best_ask_lots=q->ask_lots;out_nbbo->ask_venue_id=q->venue_id;}out_nbbo->valid=true;}return out_nbbo->best_bid_ticks<=out_nbbo->best_ask_ticks?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;}
