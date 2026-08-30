/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/valuation_adjustment.c
 *
 * PURPOSE:
 *   Represent one discounted valuation-adjustment component.
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

#include "umicom/finance/quant/valuation_adjustment.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_valuation_adjustment_init(UmiQuantValuationAdjustment *record, double exposure, double probability, double loss_given_event, double discount_factor)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(exposure >= 0.0 && probability >= 0.0 && probability <= 1.0 && loss_given_event >= 0.0 && loss_given_event <= 1.0 && discount_factor >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->exposure = exposure;
    record->probability = probability;
    record->loss_given_event = loss_given_event;
    record->discount_factor = discount_factor;
    return UMI_STATUS_OK;
}

/* Return discounted expected valuation adjustment. */
double umi_quant_valuation_adjustment_amount(const UmiQuantValuationAdjustment *record)
{
    if (record == NULL) return 0.0;
    return record->exposure * record->probability * record->loss_given_event * record->discount_factor;
}
