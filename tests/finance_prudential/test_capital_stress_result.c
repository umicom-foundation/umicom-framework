/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_stress_result.c
 *
 * PURPOSE:
 *   Verify represent post-stress capital ratio and regulatory headroom.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/capital_stress_result.h"

int main(void) { UmiPrudentialCapitalStressResult r; if(umi_pru_capital_stress_result_evaluate(&r,1.1,1.0)!=UMI_STATUS_OK)return 1; return r.passed==1&&r.headroom>0.09?0:2; }
