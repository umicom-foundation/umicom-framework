/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/retry_policy.h
 *
 * PURPOSE:
 *   Define bounded retries only for explicitly retryable infrastructure failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RETRY_POLICY
#define UMICOM_TEST_RUNTIME_RETRY_POLICY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime retry policy data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeRetryPolicy {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t maximum_attempts;
    uint64_t backoff_ms;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeRetryPolicy;

/**
 * Initialise test runtime retry policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_retry_policy_init(UmiTestRuntimeRetryPolicy *value, const char *id);
/**
 * Check that test runtime retry policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_retry_policy_validate(const UmiTestRuntimeRetryPolicy *value);
/**
 * Provide the test runtime retry policy set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_name(UmiTestRuntimeRetryPolicy *value, const char *name);
/**
 * Provide the test runtime retry policy set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_detail(UmiTestRuntimeRetryPolicy *value, const char *detail);
/**
 * Provide the test runtime retry policy set maximum attempts operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_maximum_attempts(UmiTestRuntimeRetryPolicy *value, uint64_t number);
/**
 * Provide the test runtime retry policy set backoff ms operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_retry_policy_set_backoff_ms(UmiTestRuntimeRetryPolicy *value, uint64_t number);
/**
 * Provide the test runtime retry policy touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_retry_policy_touch(UmiTestRuntimeRetryPolicy *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime retry policy same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_retry_policy_same_identity(const UmiTestRuntimeRetryPolicy *left, const UmiTestRuntimeRetryPolicy *right);

#ifdef __cplusplus
}
#endif
#endif
