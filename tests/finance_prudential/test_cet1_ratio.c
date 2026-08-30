/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_cet1_ratio.c
 *
 * PURPOSE:
 *   Verify calculate the Common Equity Tier 1 capital ratio.
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

#include "umicom/finance/prudential/cet1_ratio.h"

int main(void) { UmiPrudentialCet1Ratio r; if(umi_pru_cet1_ratio_calculate(&r,12.0,100.0)!=UMI_STATUS_OK)return 1; return r.ratio==0.12?0:2; }
