/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/media_studio/reference_product.c
 *
 * PURPOSE:
 *   Provide the Umicom Media Studio reference-product profile used by SDK conformance examples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The example delegates to the canonical product catalogue. Real product repositories can later add domain modules while retaining this same Framework contract.
 */
#include "reference_product.h"
const UmiProductProfile *umi_example_media_studio_profile(void){return umi_reference_product_media_studio();}
