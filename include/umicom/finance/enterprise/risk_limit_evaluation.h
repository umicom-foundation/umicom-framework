/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/risk_limit_evaluation.h
 *
 * PURPOSE:
 *   Evaluate observed risk against warning and hard limit thresholds.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_LIMIT_EVALUATION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_LIMIT_EVALUATION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseRiskLimitEvaluation { char limit_id[UMI_ENTERPRISE_ID_CAPACITY]; double warning_limit; double hard_limit; double observed; double utilisation; int warning; int breach; } UmiEnterpriseRiskLimitEvaluation;
/* Evaluate one finite observed risk amount against ordered non-negative limits. */
UmiStatus umi_enterprise_risk_limit_evaluate(UmiEnterpriseRiskLimitEvaluation *evaluation,const char *limit_id,double warning_limit,double hard_limit,double observed);

#ifdef __cplusplus
}
#endif

#endif
