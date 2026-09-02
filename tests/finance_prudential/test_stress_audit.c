/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_audit.c
 *
 * PURPOSE:
 *   Verify maintain an ordered stress-testing audit sequence.
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

#include "umicom/finance/prudential/stress_audit.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressAudit a={0}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_audit_append(&a,1U,11U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_audit_append(&a,1U,12U)!=UMI_STATUS_INVALID_STATE)return 2; return 0; }
