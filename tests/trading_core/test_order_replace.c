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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFinancialId id;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_id_assign(&id,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingOrderReplace v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_order_replace_init(&v,&id,1U,20,1010)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_order_replace_valid(&v)) return 2;
     return 0;
}
