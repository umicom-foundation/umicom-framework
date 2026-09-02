/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_governance.c
 *
 * PURPOSE:
 *   Verify define review, approval and sign-off state for an enterprise stress run.
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

#include "umicom/finance/prudential/stress_governance.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressGovernance g; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_governance_evaluate(&g,"RUN",1,1,1)!=UMI_STATUS_OK)return 1; return g.state==UMI_PRU_STRESS_READY?0:2; }
