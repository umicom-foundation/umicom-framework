/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/bootstrap.h
 *
 * PURPOSE:
 *   Describe the minimal product bootstrap result used by reference consumers.
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
#ifndef UMICOM_PRODUCT_BOOTSTRAP_H
#define UMICOM_PRODUCT_BOOTSTRAP_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the product bootstrap result data shared with callers of this public contract.
 */
typedef struct UmiProductBootstrapResult { const char *product_id; size_t required_capabilities; size_t enabled_frontends; int ready; } UmiProductBootstrapResult;
/**
 * Provide the product bootstrap operation used by this module and its client applications.
 */
UmiStatus umi_product_bootstrap(const UmiProductProfile *profile,UmiProductBootstrapResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
