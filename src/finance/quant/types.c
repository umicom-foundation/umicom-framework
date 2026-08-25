/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/types.c
 *
 * PURPOSE:
 *   Define common quantitative product, curve, model and scenario primitives.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/types.h"

#include <math.h>
#include <string.h>

/* Copy a complete identifier so configuration errors cannot be hidden by truncation. */
UmiStatus umi_quant_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length == 0U || length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Centralise numeric validation for all pricing modules. */
int umi_quant_number_valid(double value)
{
    return isfinite(value) ? 1 : 0;
}

/* Use the C runtime error function for a stable standard-normal CDF. */
double umi_quant_normal_cdf(double value)
{
    return 0.5 * (1.0 + erf(value / sqrt(2.0)));
}
