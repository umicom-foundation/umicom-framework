/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/scenario_result.h
 *
 * PURPOSE:
 *   Capture portfolio P&L and derived non-negative loss for one scenario.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_SCENARIO_RESULT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_SCENARIO_RESULT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseScenarioResult { char scenario_id[UMI_ENTERPRISE_ID_CAPACITY]; char portfolio_id[UMI_ENTERPRISE_ID_CAPACITY]; double pnl; double loss; } UmiEnterpriseScenarioResult;
/* Initialise a scenario result and derive loss as max(-PnL,0). */
UmiStatus umi_enterprise_scenario_result_init(UmiEnterpriseScenarioResult *result,const char *scenario_id,const char *portfolio_id,double pnl);

#ifdef __cplusplus
}
#endif

#endif
