/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/designer/reference_product.c
 *
 * PURPOSE:
 *   Provide the Umicom Designer reference-product profile used by SDK conformance examples.
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
 * Provide the example designer profile operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_example_designer_profile(void){return umi_reference_product_designer();}
