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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFinancialId id;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_id_assign(&id,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingOrderCancel v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_order_cancel_init(&v,&id,2U,7U)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_order_cancel_valid(&v)) return 2;
     return 0;
}
