/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/timeout_policy.h
 *
 * PURPOSE:
 *   Define bounded start, execution and shutdown time budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TIMEOUT_POLICY
#define UMICOM_TEST_RUNTIME_TIMEOUT_POLICY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime timeout policy data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeTimeoutPolicy {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t start_timeout_ms;
    uint64_t execution_timeout_ms;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeTimeoutPolicy;

/**
 * Initialise test runtime timeout policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_timeout_policy_init(UmiTestRuntimeTimeoutPolicy *value, const char *id);
/**
 * Check that test runtime timeout policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_timeout_policy_validate(const UmiTestRuntimeTimeoutPolicy *value);
/**
 * Provide the test runtime timeout policy set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_name(UmiTestRuntimeTimeoutPolicy *value, const char *name);
/**
 * Provide the test runtime timeout policy set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_detail(UmiTestRuntimeTimeoutPolicy *value, const char *detail);
/**
 * Provide the test runtime timeout policy set start timeout ms operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_start_timeout_ms(UmiTestRuntimeTimeoutPolicy *value, uint64_t number);
/**
 * Provide the test runtime timeout policy set execution timeout ms operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_execution_timeout_ms(UmiTestRuntimeTimeoutPolicy *value, uint64_t number);
/**
 * Provide the test runtime timeout policy touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_touch(UmiTestRuntimeTimeoutPolicy *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime timeout policy same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_timeout_policy_same_identity(const UmiTestRuntimeTimeoutPolicy *left, const UmiTestRuntimeTimeoutPolicy *right);

#ifdef __cplusplus
}
#endif
#endif
