/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_scenario.h
 *
 * PURPOSE:
 *   Represent capital scenario adjustments to earnings, losses and RWA.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_SCENARIO_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_SCENARIO_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialCapitalScenario { char scenario_id[UMI_PRU_ID_CAPACITY]; double earnings_delta; double loss_delta; double rwa_multiplier; } UmiPrudentialCapitalScenario;
/* Initialise a capital scenario with explicit earnings, loss and RWA effects. */
UmiStatus umi_pru_capital_scenario_init(UmiPrudentialCapitalScenario *scenario, const char *scenario_id, double earnings_delta, double loss_delta, double rwa_multiplier);

#ifdef __cplusplus
}
#endif

#endif
