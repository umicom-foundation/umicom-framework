/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/calibration_target.c
 *
 * PURPOSE:
 *   Represent one observed model-calibration target.
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

#include "umicom/finance/quant/calibration_target.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_calibration_target_init(UmiQuantCalibrationTarget *record, double market_value, double weight, double tolerance)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(weight >= 0.0 && tolerance >= 0.0 && umi_quant_number_valid(market_value))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->market_value = market_value;
    record->weight = weight;
    record->tolerance = tolerance;
    return UMI_STATUS_OK;
}

/* Return weighted target value for calibration objectives. */
double umi_quant_calibration_target_weighted_target(const UmiQuantCalibrationTarget *record)
{
    if (record == NULL) return 0.0;
    return record->market_value * record->weight;
}
