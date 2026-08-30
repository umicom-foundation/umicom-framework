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
int main(void){UmiTreasuryTreasuryService s;UmiTreasuryTreasurySnapshot snap;umi_treasury_treasury_service_init(&s);if(umi_treasury_treasury_snapshot_init(&snap,"snap",100,0,10,20)!=UMI_STATUS_OK)return 1;if(umi_treasury_treasury_service_start(&s,&snap)!=UMI_STATUS_OK)return 2;if(!umi_treasury_treasury_service_ready(&s))return 3;umi_treasury_treasury_service_set_blockers(&s,1U);if(umi_treasury_treasury_service_ready(&s))return 4;return 0;}
