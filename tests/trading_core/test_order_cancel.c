/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_cancel.c
 *
 * PURPOSE:
 *   Exercise describe a cancellable order request with version control and reason code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_cancel.h"
int main(void) {
    UmiFinancialId id;
     if(umi_trading_core_id_assign(&id,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingOrderCancel v;
     if(umi_trading_order_cancel_init(&v,&id,2U,7U)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_order_cancel_valid(&v)) return 2;
     return 0;
}
