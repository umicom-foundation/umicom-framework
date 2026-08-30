/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/validation.h
 *
 * PURPOSE:
 *   Validate product identifiers, branding, capabilities and profile structure.
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
#ifndef UMICOM_PRODUCT_VALIDATION_H
#define UMICOM_PRODUCT_VALIDATION_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductValidation { int descriptor_valid; int profile_valid; int capabilities_valid; int passed; } UmiProductValidation;
UmiStatus umi_product_validation_run(const UmiProductProfile *profile,UmiProductValidation *out_result);
#ifdef __cplusplus
}
#endif
#endif
