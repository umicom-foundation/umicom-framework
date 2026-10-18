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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_platform_product_validation_profile_archive_encode(const UmiTestPlatformProductValidationProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_platform_product_validation_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiTestPlatformProductValidationProfile *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTestPlatformProductValidationProfile object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_test_platform_product_validation_profile_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTestPlatformProductValidationProfile *workspace, UmiTestPlatformProductValidationProfile *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_test_platform_product_validation_profile_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTestPlatformProductValidationProfile *expected, UmiTestPlatformProductValidationProfile *value);


#ifdef __cplusplus
}
#endif
#endif
