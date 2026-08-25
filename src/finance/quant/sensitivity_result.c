/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/sensitivity_result.c
 *
 * PURPOSE:
 *   Capture base/down/up valuations for sensitivity calculation.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/sensitivity_result.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_sensitivity_result_init(UmiQuantSensitivityResult *record, double down_pv, double base_pv, double up_pv, double bump_size)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(bump_size > 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->down_pv = down_pv;
    record->base_pv = base_pv;
    record->up_pv = up_pv;
    record->bump_size = bump_size;
    return UMI_STATUS_OK;
}

/* Return central-difference delta from valuation evidence. */
double umi_quant_sensitivity_result_delta(const UmiQuantSensitivityResult *record)
{
    if (record == NULL) return 0.0;
    return (record->up_pv - record->down_pv) / (2.0 * record->bump_size);
}
