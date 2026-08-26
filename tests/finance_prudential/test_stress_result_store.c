/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_result_store.c
 *
 * PURPOSE:
 *   Verify store bounded stress results and count failed control thresholds.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_result_store.h"

int main(void) { UmiPrudentialStressResultStore s={0}; UmiPrudentialStressResult r; if(umi_pru_stress_result_evaluate(&r,"LCR",0.8,1.0)!=UMI_STATUS_OK)return 1; if(umi_pru_stress_result_store_add(&s,&r)!=UMI_STATUS_OK)return 2; return umi_pru_stress_result_store_failed_count(&s)==1U?0:3; }
