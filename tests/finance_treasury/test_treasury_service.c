/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_service.c
 *
 * PURPOSE:
 *   Verify aggregate treasury service readiness gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTreasuryTreasuryService s;UmiTreasuryTreasurySnapshot snap;umi_treasury_treasury_service_init(&s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_treasury_treasury_snapshot_init(&snap,"snap",100,0,10,20)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_treasury_treasury_service_start(&s,&snap)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_treasury_treasury_service_ready(&s))return 3;umi_treasury_treasury_service_set_blockers(&s,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_treasury_treasury_service_ready(&s))return 4;return 0;}
