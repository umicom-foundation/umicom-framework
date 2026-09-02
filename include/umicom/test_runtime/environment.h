/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/environment.h
 *
 * PURPOSE:
 *   Retain the inherited and explicit process environment used to launch tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ENVIRONMENT
#define UMICOM_TEST_RUNTIME_ENVIRONMENT

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime environment data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeEnvironment {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeEnvironment;

/**
 * Initialise test runtime environment from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_environment_init(UmiTestRuntimeEnvironment *value, const char *id);
/**
 * Check that test runtime environment satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_environment_validate(const UmiTestRuntimeEnvironment *value);
/**
 * Provide the test runtime environment set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_environment_set_name(UmiTestRuntimeEnvironment *value, const char *name);
/**
 * Provide the test runtime environment set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_environment_set_detail(UmiTestRuntimeEnvironment *value, const char *detail);
/**
 * Return the number of records represented by test runtime environment set entry without
 * changing their state.
 */
UmiStatus umi_test_runtime_environment_set_entry_count(UmiTestRuntimeEnvironment *value, uint64_t number);
/**
 * Provide the test runtime environment set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_environment_set_generation(UmiTestRuntimeEnvironment *value, uint64_t number);
/**
 * Provide the test runtime environment touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_environment_touch(UmiTestRuntimeEnvironment *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime environment same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_environment_same_identity(const UmiTestRuntimeEnvironment *left, const UmiTestRuntimeEnvironment *right);

#ifdef __cplusplus
}
#endif
#endif
