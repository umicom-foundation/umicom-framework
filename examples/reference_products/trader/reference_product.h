/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/trader/reference_product.h
 *
 * PURPOSE:
 *   Expose the Umicom Trader reference-product profile to the example executable and test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each reference product is intentionally small. It proves that independent products can consume the same Framework product contracts without duplicating the Framework itself.
 */
#ifndef UMICOM_REFERENCE_TRADER_H
#define UMICOM_REFERENCE_TRADER_H
#include "umicom/product/product.h"
const UmiProductProfile *umi_example_trader_profile(void);
#endif
