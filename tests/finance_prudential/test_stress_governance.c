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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_governance.h"

int main(void) { UmiPrudentialStressGovernance g; if(umi_pru_stress_governance_evaluate(&g,"RUN",1,1,1)!=UMI_STATUS_OK)return 1; return g.state==UMI_PRU_STRESS_READY?0:2; }
