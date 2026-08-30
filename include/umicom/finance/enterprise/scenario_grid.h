/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/scenario_grid.h
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of unique historical scenarios for distributed risk runs.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_SCENARIO_GRID_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_SCENARIO_GRID_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/historical_scenario.h"
typedef struct UmiEnterpriseScenarioGrid { UmiEnterpriseHistoricalScenario scenarios[UMI_ENTERPRISE_MAX_SCENARIOS]; size_t count; } UmiEnterpriseScenarioGrid;
/* Initialise an empty scenario grid. */
void umi_enterprise_scenario_grid_init(UmiEnterpriseScenarioGrid *grid);
/* Add a uniquely identified scenario. */
UmiStatus umi_enterprise_scenario_grid_add(UmiEnterpriseScenarioGrid *grid,const UmiEnterpriseHistoricalScenario *scenario);

#ifdef __cplusplus
}
#endif

#endif
