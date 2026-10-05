/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_product_profile.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/build_readiness/product_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/test_platform/build_readiness/product_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTestPlatformProductValidationProfileTransferEqual(const UmiTestPlatformProductValidationProfile *a, const UmiTestPlatformProductValidationProfile *b)
{
    return a->structure_size == b->structure_size &&
        a->api_version == b->api_version &&
        strcmp(a->product_id, b->product_id) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->preset, b->preset) == 0 &&
        strcmp(a->test_regex, b->test_regex) == 0 &&
        a->enabled_in_default_preset == b->enabled_in_default_preset &&
        a->requires_all_modules == b->requires_all_modules;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTestPlatformProductValidationProfileTransferTails(UmiTestPlatformProductValidationProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->product_id) + 1U;
        memset(value->product_id + used, 0xa5, sizeof(value->product_id) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->preset) + 1U;
        memset(value->preset + used, 0xa5, sizeof(value->preset) - used);
    }
    {
        size_t used = strlen(value->test_regex) + 1U;
        memset(value->test_regex + used, 0xa5, sizeof(value->test_regex) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTestPlatformProductValidationProfileTransferMalformed(const UmiTestPlatformProductValidationProfile *sample)
{
    (void)sample;
    {
        UmiTestPlatformProductValidationProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.product_id, 'x', sizeof(invalid.product_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_product_validation_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_product_validation_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated product_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestPlatformProductValidationProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_product_validation_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_product_validation_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestPlatformProductValidationProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.preset, 'x', sizeof(invalid.preset));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_product_validation_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_product_validation_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated preset was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestPlatformProductValidationProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.test_regex, 'x', sizeof(invalid.test_regex));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_product_validation_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_product_validation_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated test_regex was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTestPlatformProductValidationProfileTransferCases, UmiTestPlatformProductValidationProfile,
    umi_test_platform_product_validation_profile_archive_encode, umi_test_platform_product_validation_profile_archive_decode,
    UmiTestPlatformProductValidationProfileTransferEqual, UmiTestPlatformProductValidationProfileTransferTails, UmiTestPlatformProductValidationProfileTransferMalformed)

int main(void) {
    UmiTestPlatformProductValidationProfile profile;
    assert(umi_test_platform_product_validation_profile_init(&profile, "trader",
        "Umicom Trader", "windows-ucrt64-all-debug", "trader", false, true) ==
        UMI_STATUS_OK);
    assert(umi_test_platform_product_validation_profile_validate(&profile) ==
        UMI_STATUS_OK);
    if (UmiTestPlatformProductValidationProfileTransferCases(&profile) != 0) return 1;

    assert(strcmp(profile.preset, "windows-ucrt64-all-debug") == 0);
    profile.enabled_in_default_preset = true;
    assert(umi_test_platform_product_validation_profile_validate(&profile) ==
        UMI_STATUS_INVALID_STATE);
    return 0;
}

