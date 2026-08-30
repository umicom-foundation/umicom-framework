/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/catalogue.h
 *
 * PURPOSE:
 *   Expose the canonical Umicom reference-product catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_CATALOGUE_H
#define UMICOM_PRODUCT_CATALOGUE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/reference.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t umi_reference_product_count(void);
const UmiProductProfile *umi_reference_product_at(size_t index);
const UmiProductProfile *umi_reference_product_find(const char *product_id);
#ifdef __cplusplus
}
#endif
#endif
