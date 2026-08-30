/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_total_rwa.c
 *
 * PURPOSE:
 *   Verify aggregate credit, market and operational risk-weighted assets.
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

#include "umicom/finance/prudential/total_rwa.h"

int main(void) { UmiPrudentialTotalRwa r; if(umi_pru_total_rwa_calculate(&r,70.0,20.0,10.0)!=UMI_STATUS_OK)return 1; return r.total_rwa==100.0?0:2; }
