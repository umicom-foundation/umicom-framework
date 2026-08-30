/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/quant_pricing_result.h
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_QUANT_PRICING_RESULT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_QUANT_PRICING_RESULT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantQuantPricingResult {
    double present_value;
    double clean_price;
    double accrued;
} UmiQuantQuantPricingResult;

/* Initialise and validate the quant pricing result contract. */
UmiStatus umi_quant_quant_pricing_result_init(UmiQuantQuantPricingResult *record, double present_value, double clean_price, double accrued);

/* Return clean price plus accrued amount. */
double umi_quant_quant_pricing_result_dirty_price(const UmiQuantQuantPricingResult *record);

#ifdef __cplusplus
}
#endif

#endif
