/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_execution_fill.c
 *
 * PURPOSE:
 *   Exercise represent one integer-normalised venue fill for deterministic aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_fill.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFinancialId eid,oid;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_id_assign(&eid,"e")!=UMI_STATUS_OK||umi_trading_core_id_assign(&oid,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingExecutionFill v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_execution_fill_init(&v,&eid,&oid,5,100,1000)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_execution_fill_valid(&v)) return 2;
     return 0;
}
