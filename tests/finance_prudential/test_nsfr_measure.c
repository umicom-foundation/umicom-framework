/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_nsfr_measure.c
 *
 * PURPOSE:
 *   Verify calculate the net stable funding ratio.
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

#include "umicom/finance/prudential/nsfr_measure.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialNsfrMeasure r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_nsfr_measure_calculate(&r,12.0,100.0)!=UMI_STATUS_OK)return 1; return r.ratio==0.12?0:2; }
