/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_prudential_limit_result.c
 *
 * PURPOSE:
 *   Verify evaluate a prudential metric against a configured control limit.
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

#include "umicom/finance/prudential/prudential_limit_result.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialLimitResult r; UmiPrudentialLimit l; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_prudential_limit_init(&l,"LCR",1.0,1,UMI_PRU_SEVERITY_BREACH)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_prudential_limit_result_evaluate(&r,&l,0.9)!=UMI_STATUS_OK)return 2; return r.breached==1?0:3; }
