/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/calibration_result.h
 *
 * PURPOSE:
 *   Capture model calibration residual evidence.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CALIBRATION_RESULT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CALIBRATION_RESULT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantCalibrationResult {
    double market_value;
    double model_value;
    double weight;
} UmiQuantCalibrationResult;

/* Initialise and validate the calibration result contract. */
UmiStatus umi_quant_calibration_result_init(UmiQuantCalibrationResult *record, double market_value, double model_value, double weight);

/* Return weighted absolute model-versus-market residual. */
double umi_quant_calibration_result_weighted_error(const UmiQuantCalibrationResult *record);

#ifdef __cplusplus
}
#endif

#endif
