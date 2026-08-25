/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/survival_probability.h
 *
 * PURPOSE:
 *   Represent hazard-rate survival over a time horizon.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SURVIVAL_PROBABILITY_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SURVIVAL_PROBABILITY_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantSurvivalProbability {
    double hazard_rate;
    double years;
} UmiQuantSurvivalProbability;

/* Initialise and validate the survival probability contract. */
UmiStatus umi_quant_survival_probability_init(UmiQuantSurvivalProbability *record, double hazard_rate, double years);

/* Return exponential survival probability. */
double umi_quant_survival_probability_probability(const UmiQuantSurvivalProbability *record);

#ifdef __cplusplus
}
#endif

#endif
