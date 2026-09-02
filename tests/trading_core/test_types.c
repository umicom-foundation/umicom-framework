/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_types.c
 *
 * PURPOSE:
 *   Exercise bounded identifiers and overflow-aware arithmetic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiFinancialId id;
     int64_t x=0;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_id_assign(&id,"order-1")!=UMI_STATUS_OK)return 1;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_add_i64(10,20,&x)!=UMI_STATUS_OK||x!=30)return 2;
     return 0;
     }
