/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_consolidated_quote.c
 *
 * PURPOSE:
 *   Exercise represent a venue quote using integer ticks and lots for deterministic consolidation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/consolidated_quote.h"
int main(void) {

    UmiFinancialId i,v;
    UmiTradingConsolidatedQuote q;
    umi_trading_core_id_assign(&i,"i");
    umi_trading_core_id_assign(&v,"v");
    if(umi_trading_consolidated_quote_init(&q,&i,&v,99,10,101,12,1)!=UMI_STATUS_OK)return 1;
    return umi_trading_consolidated_quote_valid(&q)?0:2;
}
