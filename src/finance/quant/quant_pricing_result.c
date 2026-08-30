/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/quant_pricing_result.c
 *
 * PURPOSE:
 *   Capture present value and clean/dirty price evidence.
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

#include "umicom/finance/quant/quant_pricing_result.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_quant_pricing_result_init(UmiQuantQuantPricingResult *record, double present_value, double clean_price, double accrued)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(umi_quant_number_valid(present_value) && umi_quant_number_valid(clean_price) && umi_quant_number_valid(accrued))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->present_value = present_value;
    record->clean_price = clean_price;
    record->accrued = accrued;
    return UMI_STATUS_OK;
}

/* Return clean price plus accrued amount. */
double umi_quant_quant_pricing_result_dirty_price(const UmiQuantQuantPricingResult *record)
{
    if (record == NULL) return 0.0;
    return record->clean_price + record->accrued;
}
