/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/sensitivity_result.h
 *
 * PURPOSE:
 *   Capture base/down/up valuations for sensitivity calculation.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SENSITIVITY_RESULT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SENSITIVITY_RESULT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant sensitivity result data shared with callers of this public contract.
 */
typedef struct UmiQuantSensitivityResult {
    double down_pv;
    double base_pv;
    double up_pv;
    double bump_size;
} UmiQuantSensitivityResult;

/* Initialise and validate the sensitivity result contract. */
UmiStatus umi_quant_sensitivity_result_init(UmiQuantSensitivityResult *record, double down_pv, double base_pv, double up_pv, double bump_size);

/* Return central-difference delta from valuation evidence. */
double umi_quant_sensitivity_result_delta(const UmiQuantSensitivityResult *record);

#ifdef __cplusplus
}
#endif

#endif
