/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_suite.c
 *
 * PURPOSE:
 *   Verify aggregate scenario identifiers into one governed enterprise stress suite.
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

#include "umicom/finance/prudential/stress_suite.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressSuite s={0}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_suite_add(&s,"BASE")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_suite_add(&s,"BASE")!=UMI_STATUS_ALREADY_EXISTS)return 2; return 0; }
