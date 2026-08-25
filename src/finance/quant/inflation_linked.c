/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/inflation_linked.c
 *
 * PURPOSE:
 *   Represent inflation-indexed principal adjustment.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/inflation_linked.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_inflation_linked_init(UmiQuantInflationLinked *record, double base_index, double current_index, double principal)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(base_index > 0.0 && current_index > 0.0 && principal >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->base_index = base_index;
    record->current_index = current_index;
    record->principal = principal;
    return UMI_STATUS_OK;
}

/* Return principal scaled by the current/base inflation index ratio. */
double umi_quant_inflation_linked_adjusted_principal(const UmiQuantInflationLinked *record)
{
    if (record == NULL) return 0.0;
    return record->principal * record->current_index / record->base_index;
}
