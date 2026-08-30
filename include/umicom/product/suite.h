/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/suite.h
 *
 * PURPOSE:
 *   Compose independent products into an Umicom suite without merging their private state.
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
#ifndef UMICOM_PRODUCT_SUITE_H
#define UMICOM_PRODUCT_SUITE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductSuite { const char *suite_id; const UmiProductProfile *const *products; size_t product_count; } UmiProductSuite;
UmiStatus umi_product_suite_validate(const UmiProductSuite *suite);
const UmiProductProfile *umi_product_suite_find(const UmiProductSuite *suite,const char *product_id);
#ifdef __cplusplus
}
#endif
#endif
