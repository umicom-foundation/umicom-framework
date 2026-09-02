/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_hqla_bucket.c
 *
 * PURPOSE:
 *   Verify aggregate HQLA values by regulatory liquidity level.
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

#include "umicom/finance/prudential/hqla_bucket.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialHqlaBucket b={0}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_hqla_bucket_add(&b,UMI_PRU_HQLA_LEVEL1,100.0,95.0)!=UMI_STATUS_OK)return 1; return b.eligible_value==95.0?0:2; }
