/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/curve_node.c
 *
 * PURPOSE:
 *   Represent one tenor/value node used by quantitative curves.
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

#include "umicom/finance/quant/curve_node.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_curve_node_init(UmiQuantCurveNode *record, int32_t tenor_days, double value)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(tenor_days >= 0 && umi_quant_number_valid(value))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->tenor_days = tenor_days;
    record->value = value;
    return UMI_STATUS_OK;
}

/* Return the node quote for generic curve consumers. */
double umi_quant_curve_node_quoted_value(const UmiQuantCurveNode *record)
{
    if (record == NULL) return 0.0;
    return record->value;
}
