/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_scenario.h
 *
 * PURPOSE:
 *   Define one governed enterprise stress scenario and its severity.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SCENARIO_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SCENARIO_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential stress scenario data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialStressScenario { char scenario_id[UMI_PRU_ID_CAPACITY]; char name[UMI_PRU_NAME_CAPACITY]; UmiPrudentialSeverity severity; uint32_t horizon_days; } UmiPrudentialStressScenario;
/* Initialise one governed enterprise stress scenario. */
UmiStatus umi_pru_stress_scenario_init(UmiPrudentialStressScenario *scenario, const char *scenario_id, const char *display_name, UmiPrudentialSeverity severity, uint32_t horizon_days);

#ifdef __cplusplus
}
#endif

#endif
