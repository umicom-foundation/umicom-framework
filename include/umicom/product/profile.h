/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/profile.h
 *
 * PURPOSE:
 *   Combine capabilities, features and frontends into a product composition profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_PROFILE_H
#define UMICOM_PRODUCT_PROFILE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/descriptor.h"
#include "umicom/product/capability.h"
#include "umicom/product/feature.h"
#include "umicom/product/frontend.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductProfile { const UmiProductDescriptor *descriptor; const UmiProductCapability *capabilities; size_t capability_count; const UmiProductFeature *features; size_t feature_count; const UmiProductFrontend *frontends; size_t frontend_count; } UmiProductProfile;
UmiStatus umi_product_profile_validate(const UmiProductProfile *profile);
#ifdef __cplusplus
}
#endif
#endif
