/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_prudential_service.c
 *
 * PURPOSE:
 *   Verify aggregate Framework prudential service readiness and control state.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/prudential_service.h"

int main(void) { UmiPrudentialService s; if(umi_pru_prudential_service_evaluate(&s,1,1,1,1,0U)!=UMI_STATUS_OK)return 1; return umi_pru_prudential_service_ready(&s)==1?0:2; }
