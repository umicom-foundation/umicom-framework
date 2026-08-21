/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/timeout_policy.h
 *
 * PURPOSE:
 *   Define bounded start, execution and shutdown time budgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TIMEOUT_POLICY
#define UMICOM_TEST_RUNTIME_TIMEOUT_POLICY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_test_runtime_timeout_policy_init(UmiTestRuntimeTimeoutPolicy *value, const char *id);
UmiStatus umi_test_runtime_timeout_policy_validate(const UmiTestRuntimeTimeoutPolicy *value);
UmiStatus umi_test_runtime_timeout_policy_set_name(UmiTestRuntimeTimeoutPolicy *value, const char *name);
UmiStatus umi_test_runtime_timeout_policy_set_detail(UmiTestRuntimeTimeoutPolicy *value, const char *detail);
UmiStatus umi_test_runtime_timeout_policy_set_start_timeout_ms(UmiTestRuntimeTimeoutPolicy *value, uint64_t number);
UmiStatus umi_test_runtime_timeout_policy_set_execution_timeout_ms(UmiTestRuntimeTimeoutPolicy *value, uint64_t number);
UmiStatus umi_test_runtime_timeout_policy_touch(UmiTestRuntimeTimeoutPolicy *value, uint64_t updated_at_ms);
bool umi_test_runtime_timeout_policy_same_identity(const UmiTestRuntimeTimeoutPolicy *left, const UmiTestRuntimeTimeoutPolicy *right);

#ifdef __cplusplus
}
#endif
#endif
