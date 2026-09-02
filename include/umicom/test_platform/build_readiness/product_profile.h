/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/product_profile.h
 * PURPOSE: Describe the canonical preset and CTest selector for one product.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the test platform product validation profile data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise test platform product validation profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_product_validation_profile_init(
    UmiTestPlatformProductValidationProfile *profile,
    const char *product_id,
    const char *display_name,
    const char *preset,
    const char *test_regex,
    bool enabled_in_default_preset,
    bool requires_all_modules);
/**
 * Check that test platform product validation profile satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_test_platform_product_validation_profile_validate(
    const UmiTestPlatformProductValidationProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
