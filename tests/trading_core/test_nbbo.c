/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_nbbo.c
 *
 * PURPOSE:
 *   Exercise calculate a national-best-style bid and offer across bounded venue quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/nbbo.h"
int main(void) {

    UmiFinancialId i,v1,v2;
    UmiTradingConsolidatedQuote q[2];
    UmiTradingNbbo n;
    umi_trading_core_id_assign(&i,"i");
    umi_trading_core_id_assign(&v1,"v1");
    umi_trading_core_id_assign(&v2,"v2");
    umi_trading_consolidated_quote_init(&q[0],&i,&v1,99,1,103,1,1);
    umi_trading_consolidated_quote_init(&q[1],&i,&v2,100,2,102,2,2);
    if(umi_trading_nbbo_compute(q,2U,&n)!=UMI_STATUS_OK)return 1;
    return n.best_bid_ticks==100&&n.best_ask_ticks==102?0:2;
}
