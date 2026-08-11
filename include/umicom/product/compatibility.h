/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/compatibility.h
 *
 * PURPOSE:
 *   Check whether a product profile can run against the current Framework capability catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_COMPATIBILITY_H
#define UMICOM_PRODUCT_COMPATIBILITY_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductCompatibility { size_t required_capabilities; size_t available_capabilities; size_t missing_capabilities; int compatible; } UmiProductCompatibility;
UmiStatus umi_product_compatibility_check(const UmiProductProfile *profile,UmiProductCompatibility *out_result);
#ifdef __cplusplus
}
#endif
#endif
