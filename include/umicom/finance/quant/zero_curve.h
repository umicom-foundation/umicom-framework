/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/zero_curve.h
 *
 * PURPOSE:
 *   Store and interpolate bounded zero-rate term structures.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_ZERO_CURVE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_ZERO_CURVE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantZeroCurve {
    UmiQuantCurvePoint nodes[UMI_QUANT_MAX_CURVE_NODES];
    size_t count;
} UmiQuantZeroCurve;

/* Initialise an empty bounded zero curve. */
void umi_quant_zero_curve_init(UmiQuantZeroCurve *curve);
/* Add a strictly increasing tenor node. */
UmiStatus umi_quant_zero_curve_add(UmiQuantZeroCurve *curve, int32_t tenor_days, double rate);
/* Interpolate a zero rate linearly by tenor. */
UmiStatus umi_quant_zero_curve_rate(const UmiQuantZeroCurve *curve, int32_t tenor_days, double *out_rate);

#ifdef __cplusplus
}
#endif

#endif
