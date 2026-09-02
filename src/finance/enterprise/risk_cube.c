/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/risk_cube.c
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

#include "umicom/finance/enterprise/risk_cube.h"

#include <string.h>
/*
 * Initialise enterprise risk cube from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_enterprise_risk_cube_init(UmiEnterpriseRiskCube *c,size_t scenarios,size_t factors){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||scenarios==0U||factors==0U||scenarios>UMI_ENTERPRISE_MAX_SCENARIOS||factors>UMI_ENTERPRISE_MAX_FACTORS)return UMI_STATUS_INVALID_ARGUMENT; memset(c,0,sizeof *c);c->scenario_count=scenarios;c->factor_count=factors;return UMI_STATUS_OK; }
/*
 * Copy enterprise risk cube into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_enterprise_risk_cube_set(UmiEnterpriseRiskCube *c,size_t s,size_t f,double value){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||s>=c->scenario_count||f>=c->factor_count||!umi_quant_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT; c->values[s][f]=value;return UMI_STATUS_OK; }
/*
 * Provide the enterprise risk cube scenario total operation used by this module and its
 * client applications.
 */
double umi_enterprise_risk_cube_scenario_total(const UmiEnterpriseRiskCube *c,size_t s){ size_t f;double total=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||s>=c->scenario_count)return 0.0;/* Visit each bounded item once so every record receives the same rule. */ for(f=0U;f<c->factor_count;++f)total+=c->values[s][f];return total; }
