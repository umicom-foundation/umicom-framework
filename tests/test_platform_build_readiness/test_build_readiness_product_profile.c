/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_product_profile.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/build_readiness/product_profile.h"
int main(void) {
    UmiTestPlatformProductValidationProfile profile;
    assert(umi_test_platform_product_validation_profile_init(&profile, "trader",
        "Umicom Trader", "windows-ucrt64-all-debug", "trader", false, true) ==
        UMI_STATUS_OK);
    assert(umi_test_platform_product_validation_profile_validate(&profile) ==
        UMI_STATUS_OK);
    assert(strcmp(profile.preset, "windows-ucrt64-all-debug") == 0);
    profile.enabled_in_default_preset = true;
    assert(umi_test_platform_product_validation_profile_validate(&profile) ==
        UMI_STATUS_INVALID_STATE);
    return 0;
}

