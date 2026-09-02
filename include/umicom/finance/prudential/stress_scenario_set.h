/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_scenario_set.h
 *
 * PURPOSE:
 *   Maintain a bounded enterprise stress-scenario catalogue.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SCENARIO_SET_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SCENARIO_SET_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/stress_scenario.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential stress scenario set data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialStressScenarioSet { UmiPrudentialStressScenario scenarios[UMI_PRU_MAX_SCENARIOS]; size_t count; } UmiPrudentialStressScenarioSet;
/* Add a unique stress scenario to the bounded catalogue. */
UmiStatus umi_pru_stress_scenario_set_add(UmiPrudentialStressScenarioSet *set, const UmiPrudentialStressScenario *scenario);

#ifdef __cplusplus
}
#endif

#endif
