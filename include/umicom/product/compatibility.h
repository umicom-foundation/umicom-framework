/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/compatibility.h
 *
 * PURPOSE:
 *   Check whether a product profile can run against the current Framework capability catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Represent the product compatibility data shared with callers of this public contract.
 */
typedef struct UmiProductCompatibility { size_t required_capabilities; size_t available_capabilities; size_t missing_capabilities; int compatible; } UmiProductCompatibility;
/**
 * Provide the product compatibility check operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_compatibility_check(const UmiProductProfile *profile,UmiProductCompatibility *out_result);
#ifdef __cplusplus
}
#endif
#endif
