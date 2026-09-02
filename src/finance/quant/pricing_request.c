/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/pricing_request.c
 *
 * PURPOSE:
 *   Describe one product-level pricing request.
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

#include "umicom/finance/quant/pricing_request.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_pricing_request_init(UmiQuantPricingRequest *record, int32_t product_class, double quantity, double notional, int32_t valuation_day)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(product_class > 0 && product_class <= 9 && quantity >= 0.0 && notional >= 0.0 && valuation_day >= 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->product_class = product_class;
    record->quantity = quantity;
    record->notional = notional;
    record->valuation_day = valuation_day;
    return UMI_STATUS_OK;
}

/* Return quantity-scaled requested notional. */
double umi_quant_pricing_request_gross_exposure(const UmiQuantPricingRequest *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->quantity * record->notional;
}
