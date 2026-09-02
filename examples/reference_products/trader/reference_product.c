/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/trader/reference_product.c
 *
 * PURPOSE:
 *   Provide the Umicom Trader reference-product profile used by SDK conformance examples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The example delegates to the canonical product catalogue. Real product repositories can later add domain modules while retaining this same Framework contract.
 */
#include "reference_product.h"
/*
 * Provide the example trader profile operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_example_trader_profile(void){return umi_reference_product_trader();}
