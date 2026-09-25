/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/instrument/product_mapping.c
 *
 * PURPOSE:
 *   Apply Instrument Product Mapping without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/instrument/product_mapping.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_instrument_product_mapping(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "instrument.product-mapping",
        "Instrument Product Mapping",
        UMI_FINANCIAL_POLICY_COVERAGE,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
