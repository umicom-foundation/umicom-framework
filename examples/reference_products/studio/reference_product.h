/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/studio/reference_product.h
 *
 * PURPOSE:
 *   Expose the Umicom Studio IDE reference-product profile to the example executable and test.
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
#ifndef UMICOM_REFERENCE_STUDIO_H
#define UMICOM_REFERENCE_STUDIO_H
#include "umicom/product/product.h"
/**
 * Provide the example studio profile operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_example_studio_profile(void);
#endif
