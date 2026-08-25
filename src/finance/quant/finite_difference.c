/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/finite_difference.c
 *
 * PURPOSE:
 *   Capture symmetric valuation bumps for numerical derivatives.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/finite_difference.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_finite_difference_init(UmiQuantFiniteDifference *record, double down_value, double base_value, double up_value, double bump)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(bump > 0.0 && umi_quant_number_valid(down_value) && umi_quant_number_valid(base_value) && umi_quant_number_valid(up_value))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->down_value = down_value;
    record->base_value = base_value;
    record->up_value = up_value;
    record->bump = bump;
    return UMI_STATUS_OK;
}

/* Return the central first derivative from symmetric bumps. */
double umi_quant_finite_difference_first_derivative(const UmiQuantFiniteDifference *record)
{
    if (record == NULL) return 0.0;
    return (record->up_value - record->down_value) / (2.0 * record->bump);
}
