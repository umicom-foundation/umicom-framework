/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_types.c
 *
 * PURPOSE:
 *   Exercise bounded identifiers and overflow-aware arithmetic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/types.h"
int main(void) { UmiFinancialId id;
     int64_t x=0;
     if(umi_trading_core_id_assign(&id,"order-1")!=UMI_STATUS_OK)return 1;
     if(umi_trading_core_add_i64(10,20,&x)!=UMI_STATUS_OK||x!=30)return 2;
     return 0;
     }
