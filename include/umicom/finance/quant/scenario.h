/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/scenario.h
 *
 * PURPOSE:
 *   Represent one parallel rate/spot/volatility scenario.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SCENARIO_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SCENARIO_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant scenario data shared with callers of this public contract.
 */
typedef struct UmiQuantScenario {
    double rate_shift;
    double spot_shift;
    double vol_shift;
    double weight;
} UmiQuantScenario;

/* Initialise and validate the scenario contract. */
UmiStatus umi_quant_scenario_init(UmiQuantScenario *record, double rate_shift, double spot_shift, double vol_shift, double weight);

/* Return weighted absolute scenario shock magnitude. */
double umi_quant_scenario_magnitude(const UmiQuantScenario *record);

#ifdef __cplusplus
}
#endif

#endif
