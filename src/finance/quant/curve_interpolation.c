/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/curve_interpolation.c
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

#include "umicom/finance/quant/curve_interpolation.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_curve_interpolation_linear(double x0, double y0, double x1, double y1, double x, double *out_value)
{
    if (out_value == NULL || x1 == x0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_value = y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    return UMI_STATUS_OK;
}

UmiStatus umi_quant_curve_interpolation_log_linear(double x0, double y0, double x1, double y1, double x, double *out_value)
{
    double log_value;
    if (out_value == NULL || x1 == x0 || y0 <= 0.0 || y1 <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    log_value = log(y0) + (x - x0) * (log(y1) - log(y0)) / (x1 - x0);
    *out_value = exp(log_value);
    return UMI_STATUS_OK;
}
