/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/product_profile.h
 * PURPOSE: Describe the canonical preset and CTest selector for one product.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_PRODUCT_PROFILE_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_PRODUCT_PROFILE_H

#include <stdbool.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/build_readiness/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestPlatformProductValidationProfile {
    uint32_t structure_size;
    uint32_t api_version;
    char product_id[64];
    char display_name[128];
    char preset[96];
    char test_regex[192];
    bool enabled_in_default_preset;
    bool requires_all_modules;
} UmiTestPlatformProductValidationProfile;

UmiStatus umi_test_platform_product_validation_profile_init(
    UmiTestPlatformProductValidationProfile *profile,
    const char *product_id,
    const char *display_name,
    const char *preset,
    const char *test_regex,
    bool enabled_in_default_preset,
    bool requires_all_modules);
UmiStatus umi_test_platform_product_validation_profile_validate(
    const UmiTestPlatformProductValidationProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
