/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/historical_scenario.h
 *
 * PURPOSE:
 *   Represent named historical factor shocks for portfolio replay.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_HISTORICAL_SCENARIO_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_HISTORICAL_SCENARIO_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseHistoricalScenario { char scenario_id[UMI_ENTERPRISE_ID_CAPACITY]; UmiEnterpriseNamedMetric shocks[UMI_ENTERPRISE_MAX_FACTORS]; size_t count; } UmiEnterpriseHistoricalScenario;
/* Initialise an empty historical scenario. */
UmiStatus umi_enterprise_historical_scenario_init(UmiEnterpriseHistoricalScenario *scenario,const char *scenario_id);
/* Add a unique named finite factor shock. */
UmiStatus umi_enterprise_historical_scenario_add_shock(UmiEnterpriseHistoricalScenario *scenario,const char *factor_name,double shock);

#ifdef __cplusplus
}
#endif

#endif
