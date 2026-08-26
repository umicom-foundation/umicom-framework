/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_result.h
 *
 * PURPOSE:
 *   Represent one scenario metric result and pass/fail threshold.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_RESULT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_RESULT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressResult { char metric_id[UMI_PRU_ID_CAPACITY]; double stressed_value; double threshold; int passed; } UmiPrudentialStressResult;
/* Evaluate one stressed metric against a minimum threshold. */
UmiStatus umi_pru_stress_result_evaluate(UmiPrudentialStressResult *result, const char *metric_id, double stressed_value, double threshold);

#ifdef __cplusplus
}
#endif

#endif
