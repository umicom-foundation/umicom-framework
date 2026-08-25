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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/risk_cube.h"

#include <string.h>
UmiStatus umi_enterprise_risk_cube_init(UmiEnterpriseRiskCube *c,size_t scenarios,size_t factors){ if(c==NULL||scenarios==0U||factors==0U||scenarios>UMI_ENTERPRISE_MAX_SCENARIOS||factors>UMI_ENTERPRISE_MAX_FACTORS)return UMI_STATUS_INVALID_ARGUMENT; memset(c,0,sizeof *c);c->scenario_count=scenarios;c->factor_count=factors;return UMI_STATUS_OK; }
UmiStatus umi_enterprise_risk_cube_set(UmiEnterpriseRiskCube *c,size_t s,size_t f,double value){ if(c==NULL||s>=c->scenario_count||f>=c->factor_count||!umi_quant_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT; c->values[s][f]=value;return UMI_STATUS_OK; }
double umi_enterprise_risk_cube_scenario_total(const UmiEnterpriseRiskCube *c,size_t s){ size_t f;double total=0.0;if(c==NULL||s>=c->scenario_count)return 0.0;for(f=0U;f<c->factor_count;++f)total+=c->values[s][f];return total; }
