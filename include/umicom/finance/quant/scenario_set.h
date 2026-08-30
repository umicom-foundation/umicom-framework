/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/scenario_set.h
 *
 * PURPOSE:
 *   Store bounded deterministic market scenarios.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SCENARIO_SET_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SCENARIO_SET_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantScenarioPoint { double rate_shift; double spot_shift; double vol_shift; double weight; } UmiQuantScenarioPoint;
typedef struct UmiQuantScenarioSet { UmiQuantScenarioPoint scenarios[UMI_QUANT_MAX_SCENARIOS]; size_t count; } UmiQuantScenarioSet;
/* Initialise an empty scenario set. */
void umi_quant_scenario_set_init(UmiQuantScenarioSet *set);
/* Add one weighted scenario. */
UmiStatus umi_quant_scenario_set_add(UmiQuantScenarioSet *set, double rate_shift, double spot_shift, double vol_shift, double weight);
/* Return total scenario weight. */
double umi_quant_scenario_set_total_weight(const UmiQuantScenarioSet *set);

#ifdef __cplusplus
}
#endif

#endif
