/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/sensitivity_request.h
 *
 * PURPOSE:
 *   Describe a finite-risk-factor sensitivity bump request.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SENSITIVITY_REQUEST_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SENSITIVITY_REQUEST_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantSensitivityRequest {
    double base_value;
    double bump_size;
    int32_t central;
} UmiQuantSensitivityRequest;

/* Initialise and validate the sensitivity request contract. */
UmiStatus umi_quant_sensitivity_request_init(UmiQuantSensitivityRequest *record, double base_value, double bump_size, int32_t central);

/* Return the upward bumped market value. */
double umi_quant_sensitivity_request_up_value(const UmiQuantSensitivityRequest *record);

#ifdef __cplusplus
}
#endif

#endif
