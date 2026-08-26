/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_total_capital_ratio.c
 *
 * PURPOSE:
 *   Verify calculate the total regulatory capital ratio.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/total_capital_ratio.h"

int main(void) { UmiPrudentialTotalCapitalRatio r; if(umi_pru_total_capital_ratio_calculate(&r,12.0,100.0)!=UMI_STATUS_OK)return 1; return r.ratio==0.12?0:2; }
