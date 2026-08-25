/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/sensitivity_request.c
 *
 * PURPOSE:
 *   Describe a finite-risk-factor sensitivity bump request.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/sensitivity_request.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_sensitivity_request_init(UmiQuantSensitivityRequest *record, double base_value, double bump_size, int32_t central)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(umi_quant_number_valid(base_value) && bump_size > 0.0 && (central == 0 || central == 1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->base_value = base_value;
    record->bump_size = bump_size;
    record->central = central;
    return UMI_STATUS_OK;
}

/* Return the upward bumped market value. */
double umi_quant_sensitivity_request_up_value(const UmiQuantSensitivityRequest *record)
{
    if (record == NULL) return 0.0;
    return record->base_value + record->bump_size;
}
