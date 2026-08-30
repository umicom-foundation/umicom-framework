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

int main(void) { UmiPrudentialStressAudit a={0}; if(umi_pru_stress_audit_append(&a,1U,11U)!=UMI_STATUS_OK)return 1; if(umi_pru_stress_audit_append(&a,1U,12U)!=UMI_STATUS_INVALID_STATE)return 2; return 0; }
