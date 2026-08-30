/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_result.c
 *
 * PURPOSE:
 *   Verify represent one scenario metric result and pass/fail threshold.
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

#include "umicom/finance/prudential/stress_result.h"

int main(void) { UmiPrudentialStressResult r; if(umi_pru_stress_result_evaluate(&r,"CET1",0.11,0.08)!=UMI_STATUS_OK)return 1; return r.passed==1?0:2; }
