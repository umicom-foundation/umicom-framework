/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/profile.h
 *
 * PURPOSE:
 *   Define one named execution profile such as smoke, data, designer or Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROFILE
#define UMICOM_TEST_RUNTIME_PROFILE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime profile data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeProfile {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t test_count;
    uint64_t maximum_parallel;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeProfile;

/**
 * Initialise test runtime profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_runtime_profile_init(UmiTestRuntimeProfile *value, const char *id);
/**
 * Check that test runtime profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_profile_validate(const UmiTestRuntimeProfile *value);
/**
 * Provide the test runtime profile set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_profile_set_name(UmiTestRuntimeProfile *value, const char *name);
/**
 * Provide the test runtime profile set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_profile_set_detail(UmiTestRuntimeProfile *value, const char *detail);
/**
 * Return the number of records represented by test runtime profile set test without
 * changing their state.
 */
UmiStatus umi_test_runtime_profile_set_test_count(UmiTestRuntimeProfile *value, uint64_t number);
/**
 * Provide the test runtime profile set maximum parallel operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_profile_set_maximum_parallel(UmiTestRuntimeProfile *value, uint64_t number);
/**
 * Provide the test runtime profile touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_profile_touch(UmiTestRuntimeProfile *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime profile same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_profile_same_identity(const UmiTestRuntimeProfile *left, const UmiTestRuntimeProfile *right);

#ifdef __cplusplus
}
#endif
#endif
