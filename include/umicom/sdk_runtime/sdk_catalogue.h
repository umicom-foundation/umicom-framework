/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/sdk_catalogue.h
 *
 * PURPOSE:
 *   Maintain reusable SDK profiles for first-party and external consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SDK_CATALOGUE
#define UMICOM_SDK_RUNTIME_SDK_CATALOGUE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime sdk catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeSdkCatalogue {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t profile_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeSdkCatalogue;
/**
 * Initialise sdk runtime sdk catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_sdk_catalogue_init(UmiSdkRuntimeSdkCatalogue *value, const char *id);
/**
 * Check that sdk runtime sdk catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_sdk_catalogue_validate(const UmiSdkRuntimeSdkCatalogue *value);
/**
 * Provide the sdk runtime sdk catalogue set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_sdk_catalogue_set_path(UmiSdkRuntimeSdkCatalogue *value, const char *path);
/**
 * Provide the sdk runtime sdk catalogue set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_sdk_catalogue_set_detail(UmiSdkRuntimeSdkCatalogue *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime sdk catalogue set profile
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_sdk_catalogue_set_profile_count(UmiSdkRuntimeSdkCatalogue *value, uint64_t number);
/**
 * Provide the sdk runtime sdk catalogue set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_sdk_catalogue_set_generation(UmiSdkRuntimeSdkCatalogue *value, uint64_t number);
/**
 * Provide the sdk runtime sdk catalogue set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_sdk_catalogue_set_state(UmiSdkRuntimeSdkCatalogue *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime sdk catalogue same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_sdk_catalogue_same_identity(const UmiSdkRuntimeSdkCatalogue *left, const UmiSdkRuntimeSdkCatalogue *right);

/** Publish a complete reviewed value while expected_revision still matches.
 * Copy the live record, edit that copy with the ordinary setters, and retain
 * the revision observed before editing. Both values must validate and have
 * the same ID. A stale review returns INVALID_STATE; exhausted revision space
 * returns CAPACITY_EXCEEDED. All failures leave the live value unchanged.
 * Success copies every proposal field and assigns one new live revision;
 * the proposal's own revision does not control publication. Self-assignment
 * is allowed and also advances once. Other overlapping storage is unsupported.
 * Serialize access on the owner. This value operation allocates nothing and
 * does not perform I/O, authenticate evidence or run the described workflow. */
UmiStatus umi_sdk_runtime_sdk_catalogue_replace_if_current(UmiSdkRuntimeSdkCatalogue *value,
    uint64_t expected_revision, const UmiSdkRuntimeSdkCatalogue *proposal);

#ifdef __cplusplus
}
#endif
#endif
