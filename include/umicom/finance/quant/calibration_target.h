/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/calibration_target.h
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CALIBRATION_TARGET_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CALIBRATION_TARGET_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant calibration target data shared with callers of this public contract.
 */
typedef struct UmiQuantCalibrationTarget {
    double market_value;
    double weight;
    double tolerance;
} UmiQuantCalibrationTarget;

/* Initialise and validate the calibration target contract. */
UmiStatus umi_quant_calibration_target_init(UmiQuantCalibrationTarget *record, double market_value, double weight, double tolerance);

/* Return weighted target value for calibration objectives. */
double umi_quant_calibration_target_weighted_target(const UmiQuantCalibrationTarget *record);

#ifdef __cplusplus
}
#endif

#endif
