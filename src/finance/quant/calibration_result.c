/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/calibration_result.c
 *
 * PURPOSE:
 *   Capture model calibration residual evidence.
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

#include "umicom/finance/quant/calibration_result.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_calibration_result_init(UmiQuantCalibrationResult *record, double market_value, double model_value, double weight)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(weight >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->market_value = market_value;
    record->model_value = model_value;
    record->weight = weight;
    return UMI_STATUS_OK;
}

/* Return weighted absolute model-versus-market residual. */
double umi_quant_calibration_result_weighted_error(const UmiQuantCalibrationResult *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return fabs(record->model_value - record->market_value) * record->weight;
}
