/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_exposure_bucket.c
 *
 * PURPOSE:
 *   Verify aggregate exposure amounts for one prudential class.
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

#include "umicom/finance/prudential/exposure_bucket.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialExposureBucket b; UmiCurrency c={"GBP"}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_exposure_bucket_init(&b,"B",10.0,c)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_exposure_bucket_add(&b,5.0)!=UMI_STATUS_OK)return 2; return b.amount==15.0?0:3; }
