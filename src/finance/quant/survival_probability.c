/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/survival_probability.c
 *
 * PURPOSE:
 *   Represent hazard-rate survival over a time horizon.
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

#include "umicom/finance/quant/survival_probability.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_survival_probability_init(UmiQuantSurvivalProbability *record, double hazard_rate, double years)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(hazard_rate >= 0.0 && years >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->hazard_rate = hazard_rate;
    record->years = years;
    return UMI_STATUS_OK;
}

/* Return exponential survival probability. */
double umi_quant_survival_probability_probability(const UmiQuantSurvivalProbability *record)
{
    if (record == NULL) return 0.0;
    return exp(-record->hazard_rate * record->years);
}
