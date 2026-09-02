/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/stress_scenario.h
 *
 * PURPOSE:
 *   Represent a named severity-scaled stress factor shock.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_STRESS_SCENARIO_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_STRESS_SCENARIO_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise stress scenario data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseStressScenario { char scenario_id[UMI_ENTERPRISE_ID_CAPACITY]; char factor_name[UMI_ENTERPRISE_NAME_CAPACITY]; double shock; int severity; } UmiEnterpriseStressScenario;
/* Initialise a stress scenario with severity in the range 1..5. */
UmiStatus umi_enterprise_stress_scenario_init(UmiEnterpriseStressScenario *scenario,const char *scenario_id,const char *factor_name,double shock,int severity);

#ifdef __cplusplus
}
#endif

#endif
