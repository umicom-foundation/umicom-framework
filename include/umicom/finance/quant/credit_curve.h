/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/credit_curve.h
 *
 * PURPOSE:
 *   Store bounded hazard-rate term structures.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CREDIT_CURVE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CREDIT_CURVE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantCreditCurve { UmiQuantCurvePoint hazards[UMI_QUANT_MAX_CURVE_NODES]; size_t count; } UmiQuantCreditCurve;
/* Initialise an empty credit curve. */
void umi_quant_credit_curve_init(UmiQuantCreditCurve *curve);
/* Add a non-negative hazard-rate node in increasing tenor order. */
UmiStatus umi_quant_credit_curve_add(UmiQuantCreditCurve *curve, int32_t tenor_days, double hazard_rate);
/* Return the piecewise-constant hazard rate at a tenor. */
UmiStatus umi_quant_credit_curve_hazard(const UmiQuantCreditCurve *curve, int32_t tenor_days, double *out_hazard);

#ifdef __cplusplus
}
#endif

#endif
