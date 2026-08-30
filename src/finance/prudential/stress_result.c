/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_result.c
 *
 * PURPOSE:
 *   Represent one scenario metric result and pass/fail threshold.
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

#include "umicom/finance/prudential/stress_result.h"

#include <string.h>
UmiStatus umi_pru_stress_result_evaluate(UmiPrudentialStressResult *result,const char *metric_id,double stressed_value,double threshold) { UmiStatus s; if(result==NULL||metric_id==NULL||!umi_pru_number_valid(stressed_value)||!umi_pru_number_valid(threshold))return UMI_STATUS_INVALID_ARGUMENT; memset(result,0,sizeof *result); s=umi_pru_copy_text(result->metric_id,sizeof result->metric_id,metric_id); if(s!=UMI_STATUS_OK)return s; result->stressed_value=stressed_value; result->threshold=threshold; result->passed=stressed_value>=threshold?1:0; return UMI_STATUS_OK; }
