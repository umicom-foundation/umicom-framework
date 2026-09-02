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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/prudential_service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialService s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_prudential_service_evaluate(&s,1,1,1,1,0U)!=UMI_STATUS_OK)return 1; return umi_pru_prudential_service_ready(&s)==1?0:2; }
