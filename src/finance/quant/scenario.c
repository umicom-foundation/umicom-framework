/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/scenario.c
 *
 * PURPOSE:
 *   Represent one parallel rate/spot/volatility scenario.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/scenario.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_scenario_init(UmiQuantScenario *record, double rate_shift, double spot_shift, double vol_shift, double weight)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(weight >= 0.0 && umi_quant_number_valid(rate_shift) && umi_quant_number_valid(spot_shift) && umi_quant_number_valid(vol_shift))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->rate_shift = rate_shift;
    record->spot_shift = spot_shift;
    record->vol_shift = vol_shift;
    record->weight = weight;
    return UMI_STATUS_OK;
}

/* Return weighted absolute scenario shock magnitude. */
double umi_quant_scenario_magnitude(const UmiQuantScenario *record)
{
    if (record == NULL) return 0.0;
    return record->weight * (fabs(record->rate_shift) + fabs(record->spot_shift) + fabs(record->vol_shift));
}
