/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/capability.h
 *
 * PURPOSE:
 *   Describe a Framework capability required or optionally used by a product.
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
#ifndef UMICOM_PRODUCT_CAPABILITY_H
#define UMICOM_PRODUCT_CAPABILITY_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductCapability { const char *capability_id; int required; } UmiProductCapability;
UmiStatus umi_product_capability_validate(const UmiProductCapability *capability);
#ifdef __cplusplus
}
#endif
#endif
