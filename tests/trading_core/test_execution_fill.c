/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_execution_fill.c
 *
 * PURPOSE:
 *   Exercise represent one integer-normalised venue fill for deterministic aggregation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_fill.h"
int main(void) {
    UmiFinancialId eid,oid;
     if(umi_trading_core_id_assign(&eid,"e")!=UMI_STATUS_OK||umi_trading_core_id_assign(&oid,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingExecutionFill v;
     if(umi_trading_execution_fill_init(&v,&eid,&oid,5,100,1000)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_execution_fill_valid(&v)) return 2;
     return 0;
}
