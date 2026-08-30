/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_replace.c
 *
 * PURPOSE:
 *   Exercise describe a replacement request with optimistic order-version control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_replace.h"
int main(void) {
    UmiFinancialId id;
     if(umi_trading_core_id_assign(&id,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingOrderReplace v;
     if(umi_trading_order_replace_init(&v,&id,1U,20,1010)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_order_replace_valid(&v)) return 2;
     return 0;
}
