/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/pricing_request.h
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_PRICING_REQUEST_H
#define INCLUDE_UMICOM_FINANCE_QUANT_PRICING_REQUEST_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant pricing request data shared with callers of this public contract.
 */
typedef struct UmiQuantPricingRequest {
    int32_t product_class;
    double quantity;
    double notional;
    int32_t valuation_day;
} UmiQuantPricingRequest;

/* Initialise and validate the pricing request contract. */
UmiStatus umi_quant_pricing_request_init(UmiQuantPricingRequest *record, int32_t product_class, double quantity, double notional, int32_t valuation_day);

/* Return quantity-scaled requested notional. */
double umi_quant_pricing_request_gross_exposure(const UmiQuantPricingRequest *record);

#ifdef __cplusplus
}
#endif

#endif
