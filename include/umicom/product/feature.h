/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/feature.h
 *
 * PURPOSE:
 *   Describe a named product feature independently from its user interface.
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
#ifndef UMICOM_PRODUCT_FEATURE_H
#define UMICOM_PRODUCT_FEATURE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductFeature { const char *feature_id; const char *description; int enabled; } UmiProductFeature;
UmiStatus umi_product_feature_validate(const UmiProductFeature *feature);
#ifdef __cplusplus
}
#endif
#endif
