/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/retry_policy.h
 *
 * PURPOSE:
 *   Define bounded retries only for explicitly retryable infrastructure failures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RETRY_POLICY
#define UMICOM_TEST_RUNTIME_RETRY_POLICY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_test_runtime_retry_policy_init(UmiTestRuntimeRetryPolicy *value, const char *id);
UmiStatus umi_test_runtime_retry_policy_validate(const UmiTestRuntimeRetryPolicy *value);
UmiStatus umi_test_runtime_retry_policy_set_name(UmiTestRuntimeRetryPolicy *value, const char *name);
UmiStatus umi_test_runtime_retry_policy_set_detail(UmiTestRuntimeRetryPolicy *value, const char *detail);
UmiStatus umi_test_runtime_retry_policy_set_maximum_attempts(UmiTestRuntimeRetryPolicy *value, uint64_t number);
UmiStatus umi_test_runtime_retry_policy_set_backoff_ms(UmiTestRuntimeRetryPolicy *value, uint64_t number);
UmiStatus umi_test_runtime_retry_policy_touch(UmiTestRuntimeRetryPolicy *value, uint64_t updated_at_ms);
bool umi_test_runtime_retry_policy_same_identity(const UmiTestRuntimeRetryPolicy *left, const UmiTestRuntimeRetryPolicy *right);

#ifdef __cplusplus
}
#endif
#endif
