/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_prudential_entity.c
 *
 * PURPOSE:
 *   Verify represent a regulated legal entity and its prudential reporting perimeter.
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

#include "umicom/finance/prudential/prudential_entity.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialEntity r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_pru_prudential_entity_init(&r,"BANK-1","Umicom Bank","GB") != UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (r.entity_id[0] != 'B' || r.jurisdiction[0] != 'G') return 2; return 0; }
