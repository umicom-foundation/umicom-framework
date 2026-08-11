/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/studio/reference_product.c
 *
 * PURPOSE:
 *   Provide the Umicom Studio IDE reference-product profile used by SDK conformance examples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The example delegates to the canonical product catalogue. Real product repositories can later add domain modules while retaining this same Framework contract.
 */
#include "reference_product.h"
const UmiProductProfile *umi_example_studio_profile(void){return umi_reference_product_studio();}
