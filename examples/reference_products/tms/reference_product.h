/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/tms/reference_product.h
 *
 * PURPOSE:
 *   Expose the Umicom Treasury Management System reference-product profile to the example executable and test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each reference product is intentionally small. It proves that independent products can consume the same Framework product contracts without duplicating the Framework itself.
 */
#ifndef UMICOM_REFERENCE_TMS_H
#define UMICOM_REFERENCE_TMS_H
#include "umicom/product/product.h"
const UmiProductProfile *umi_example_tms_profile(void);
#endif
