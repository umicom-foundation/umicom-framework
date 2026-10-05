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
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL || source[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise test platform product validation profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_product_validation_profile_init(
    UmiTestPlatformProductValidationProfile *profile, const char *product_id,
    const char *display_name, const char *preset, const char *test_regex,
    bool enabled_in_default_preset, bool requires_all_modules)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Check that test platform product validation profile satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_test_platform_product_validation_profile_validate(
    const UmiTestPlatformProductValidationProfile *profile)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->product_id, '\0', sizeof(profile->product_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->display_name, '\0', sizeof(profile->display_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->preset, '\0', sizeof(profile->preset)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(profile->test_regex, '\0', sizeof(profile->test_regex)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || profile->structure_size != sizeof(*profile) ||
        profile->api_version != UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION ||
        profile->product_id[0] == '\0' || profile->display_name[0] == '\0' ||
        profile->preset[0] == '\0' || profile->test_regex[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (profile->enabled_in_default_preset && profile->requires_all_modules)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTestPlatformProductValidationProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa67ec2c8e0d78417);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformProductValidationProfile *)0)->product_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformProductValidationProfile *)0)->display_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformProductValidationProfile *)0)->preset)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformProductValidationProfile *)0)->test_regex)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTestPlatformProductValidationProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiTestPlatformProductValidationProfile *)0)->product_id) - 1U +
        8U + sizeof(((UmiTestPlatformProductValidationProfile *)0)->display_name) - 1U +
        8U + sizeof(((UmiTestPlatformProductValidationProfile *)0)->preset) - 1U +
        8U + sizeof(((UmiTestPlatformProductValidationProfile *)0)->test_regex) - 1U +
        8U +
        8U;
}
static void UmiTestPlatformProductValidationProfileArchiveWrite(UmiArchiveWriter *writer, const UmiTestPlatformProductValidationProfile *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->product_id, sizeof(value->product_id));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteText(writer, value->preset, sizeof(value->preset));
    UmiArchiveWriteText(writer, value->test_regex, sizeof(value->test_regex));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled_in_default_preset);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->requires_all_modules);
}
static void UmiTestPlatformProductValidationProfileArchiveRead(UmiArchiveReader *reader, UmiTestPlatformProductValidationProfile *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->product_id, sizeof(value->product_id));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    UmiArchiveReadText(reader, value->preset, sizeof(value->preset));
    UmiArchiveReadText(reader, value->test_regex, sizeof(value->test_regex));
    value->enabled_in_default_preset = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->requires_all_modules = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTestPlatformProductValidationProfileArchiveValidate(const UmiTestPlatformProductValidationProfile *value)
{
    return umi_test_platform_product_validation_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_platform_product_validation_profile_archive_encode, umi_test_platform_product_validation_profile_archive_decode,
    UmiTestPlatformProductValidationProfile, UmiTestPlatformProductValidationProfileArchiveSchema, UmiTestPlatformProductValidationProfileArchiveBound, UmiTestPlatformProductValidationProfileArchiveWrite, UmiTestPlatformProductValidationProfileArchiveRead, UmiTestPlatformProductValidationProfileArchiveValidate)
