/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/product_profile.c
 * PURPOSE: Construct portable product validation profiles.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/product_profile.h"

#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || source[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_product_validation_profile_init(
    UmiTestPlatformProductValidationProfile *profile, const char *product_id,
    const char *display_name, const char *preset, const char *test_regex,
    bool enabled_in_default_preset, bool requires_all_modules)
{
    UmiStatus status;
    if (profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(profile, 0, sizeof(*profile));
    profile->structure_size = (uint32_t)sizeof(*profile);
    profile->api_version = UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION;
#define COPY_PROFILE(field, value)                                              \
    do {                                                                         \
        status = copy_text(profile->field, sizeof(profile->field), value);       \
        if (status != UMI_STATUS_OK) return status;                              \
    } while (0)
    COPY_PROFILE(product_id, product_id);
    COPY_PROFILE(display_name, display_name);
    COPY_PROFILE(preset, preset);
    COPY_PROFILE(test_regex, test_regex);
#undef COPY_PROFILE
    profile->enabled_in_default_preset = enabled_in_default_preset;
    profile->requires_all_modules = requires_all_modules;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_product_validation_profile_validate(
    const UmiTestPlatformProductValidationProfile *profile)
{
    if (profile == NULL || profile->structure_size != sizeof(*profile) ||
        profile->api_version != UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION ||
        profile->product_id[0] == '\0' || profile->display_name[0] == '\0' ||
        profile->preset[0] == '\0' || profile->test_regex[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (profile->enabled_in_default_preset && profile->requires_all_modules)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
