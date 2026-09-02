/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_evidence.c
 *
 * PURPOSE:
 *   Verify fingerprint immutable stress-test evidence by source and sequence.
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

#include "umicom/finance/prudential/stress_evidence.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressEvidence e; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_evidence_init(&e,1U,"market-snapshot")!=UMI_STATUS_OK)return 1; return e.fingerprint!=0U?0:2; }
