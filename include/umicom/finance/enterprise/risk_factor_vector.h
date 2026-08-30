/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/risk_factor_vector.h
 *
 * PURPOSE:
 *   Store bounded named risk-factor observations with deterministic lookup.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_FACTOR_VECTOR_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_FACTOR_VECTOR_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseRiskFactorVector { UmiEnterpriseNamedMetric factors[UMI_ENTERPRISE_MAX_FACTORS]; size_t count; } UmiEnterpriseRiskFactorVector;
/* Initialise an empty risk-factor vector. */
void umi_enterprise_risk_factor_vector_init(UmiEnterpriseRiskFactorVector *vector);
/* Add a unique named factor. */
UmiStatus umi_enterprise_risk_factor_vector_add(UmiEnterpriseRiskFactorVector *vector,const char *name,double value);
/* Retrieve one named factor value. */
UmiStatus umi_enterprise_risk_factor_vector_get(const UmiEnterpriseRiskFactorVector *vector,const char *name,double *out_value);

#ifdef __cplusplus
}
#endif

#endif
