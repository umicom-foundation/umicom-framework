/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_guard.h
 *
 * PURPOSE:
 *   Enforce launch preconditions before a test process is started.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_GUARD
#define UMICOM_TEST_RUNTIME_EXECUTION_GUARD
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime execution guard data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutionGuard {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t guard_count;
    uint64_t rejected_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeExecutionGuard;
/**
 * Initialise test runtime execution guard from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_execution_guard_init(UmiTestRuntimeExecutionGuard *value,const char *id);
/**
 * Check that test runtime execution guard satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_execution_guard_validate(const UmiTestRuntimeExecutionGuard *value);
/**
 * Provide the test runtime execution guard set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_guard_set_category(UmiTestRuntimeExecutionGuard *value,const char *category);
/**
 * Provide the test runtime execution guard set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_guard_set_detail(UmiTestRuntimeExecutionGuard *value,const char *detail);
/**
 * Return the number of records represented by test runtime execution guard set guard
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_guard_set_guard_count(UmiTestRuntimeExecutionGuard *value,uint64_t number);
/**
 * Return the number of records represented by test runtime execution guard set rejected
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_guard_set_rejected_count(UmiTestRuntimeExecutionGuard *value,uint64_t number);
/**
 * Provide the test runtime execution guard set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_guard_set_active(UmiTestRuntimeExecutionGuard *value,bool active);
/**
 * Provide the test runtime execution guard same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_execution_guard_same_identity(const UmiTestRuntimeExecutionGuard *left,const UmiTestRuntimeExecutionGuard *right);
#ifdef __cplusplus
}
#endif
#endif
