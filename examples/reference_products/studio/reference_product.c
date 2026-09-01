/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/studio/reference_product.c
 *
 * PURPOSE:
 *   Provide the Umicom Studio IDE reference-product profile used by SDK conformance examples.
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
const UmiProductProfile *umi_example_studio_profile(void){return umi_reference_product_studio();}
