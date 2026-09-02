/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/profile_catalogue.h
 *
 * PURPOSE:
 *   Maintain the standard Framework execution-profile catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROFILE_CATALOGUE
#define UMICOM_TEST_RUNTIME_PROFILE_CATALOGUE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime profile catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeProfileCatalogue {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t profile_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeProfileCatalogue;

/**
 * Initialise test runtime profile catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_profile_catalogue_init(UmiTestRuntimeProfileCatalogue *value, const char *id);
/**
 * Check that test runtime profile catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_profile_catalogue_validate(const UmiTestRuntimeProfileCatalogue *value);
/**
 * Provide the test runtime profile catalogue set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_profile_catalogue_set_name(UmiTestRuntimeProfileCatalogue *value, const char *name);
/**
 * Provide the test runtime profile catalogue set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_profile_catalogue_set_detail(UmiTestRuntimeProfileCatalogue *value, const char *detail);
/**
 * Return the number of records represented by test runtime profile catalogue set profile
 * without changing their state.
 */
UmiStatus umi_test_runtime_profile_catalogue_set_profile_count(UmiTestRuntimeProfileCatalogue *value, uint64_t number);
/**
 * Provide the test runtime profile catalogue set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_profile_catalogue_set_generation(UmiTestRuntimeProfileCatalogue *value, uint64_t number);
/**
 * Provide the test runtime profile catalogue touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_profile_catalogue_touch(UmiTestRuntimeProfileCatalogue *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime profile catalogue same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_profile_catalogue_same_identity(const UmiTestRuntimeProfileCatalogue *left, const UmiTestRuntimeProfileCatalogue *right);

#ifdef __cplusplus
}
#endif
#endif
