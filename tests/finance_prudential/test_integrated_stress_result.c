/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_integrated_stress_result.c
 *
 * PURPOSE:
 *   Verify combine capital and liquidity stress outcomes into one control decision.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/integrated_stress_result.h"

int main(void) { UmiPrudentialIntegratedStressResult r; if(umi_pru_integrated_stress_result_evaluate(&r,1,0)!=UMI_STATUS_OK)return 1; return r.overall_passed==0&&r.severity==UMI_PRU_SEVERITY_BREACH?0:2; }
