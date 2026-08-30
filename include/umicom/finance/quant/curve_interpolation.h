/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/curve_interpolation.h
 *
 * PURPOSE:
 *   Provide reusable linear and log-linear curve interpolation.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CURVE_INTERPOLATION_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CURVE_INTERPOLATION_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Interpolate a scalar linearly between two x coordinates. */
UmiStatus umi_quant_curve_interpolation_linear(double x0, double y0, double x1, double y1, double x, double *out_value);
/* Interpolate positive ordinates in log space. */
UmiStatus umi_quant_curve_interpolation_log_linear(double x0, double y0, double x1, double y1, double x, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
