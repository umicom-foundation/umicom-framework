/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_result_store.c
 *
 * PURPOSE:
 *   Verify store bounded stress results and count failed control thresholds.
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

#include "umicom/finance/prudential/stress_result_store.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressResultStore s={0}; UmiPrudentialStressResult r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_result_evaluate(&r,"LCR",0.8,1.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_result_store_add(&s,&r)!=UMI_STATUS_OK)return 2; return umi_pru_stress_result_store_failed_count(&s)==1U?0:3; }
