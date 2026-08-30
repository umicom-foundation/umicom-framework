/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/risk_cube.h
 *
 * PURPOSE:
 *   Store scenario-by-factor risk values in a bounded enterprise risk cube.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_CUBE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_CUBE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseRiskCube { size_t scenario_count; size_t factor_count; double values[UMI_ENTERPRISE_MAX_SCENARIOS][UMI_ENTERPRISE_MAX_FACTORS]; } UmiEnterpriseRiskCube;
/* Initialise a bounded risk cube with explicit dimensions. */
UmiStatus umi_enterprise_risk_cube_init(UmiEnterpriseRiskCube *cube,size_t scenario_count,size_t factor_count);
/* Set one finite risk-cube cell. */
UmiStatus umi_enterprise_risk_cube_set(UmiEnterpriseRiskCube *cube,size_t scenario_index,size_t factor_index,double value);
/* Sum one scenario row across all factors. */
double umi_enterprise_risk_cube_scenario_total(const UmiEnterpriseRiskCube *cube,size_t scenario_index);

#ifdef __cplusplus
}
#endif

#endif
