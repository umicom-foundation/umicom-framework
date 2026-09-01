/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/designer/reference_product.h
 *
 * PURPOSE:
 *   Expose the Umicom Designer reference-product profile to the example executable and test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Each reference product is intentionally small. It proves that independent products can consume the same Framework product contracts without duplicating the Framework itself.
 */
#ifndef UMICOM_REFERENCE_DESIGNER_H
#define UMICOM_REFERENCE_DESIGNER_H
#include "umicom/product/product.h"
const UmiProductProfile *umi_example_designer_profile(void);
#endif
