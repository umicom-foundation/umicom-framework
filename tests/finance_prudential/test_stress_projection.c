/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_projection.c
 *
 * PURPOSE:
 *   Verify project a base value across a bounded stress path.
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

#include "umicom/finance/prudential/stress_projection.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressProjection r; double m[2]={1.0,0.8}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_projection_calculate(&r,100.0,m,2U)!=UMI_STATUS_OK)return 1; return r.values[1]==80.0?0:2; }
