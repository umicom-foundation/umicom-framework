/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_exposure_class.c
 *
 * PURPOSE:
 *   Verify describe one prudential exposure class and default risk weight.
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

#include "umicom/finance/prudential/exposure_class.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialExposureClass r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_exposure_class_init(&r,"CORP",1.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(r.risk_weight!=1.0)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_exposure_class_init(&r,"BAD",13.0)!=UMI_STATUS_INVALID_ARGUMENT)return 3; return 0; }
