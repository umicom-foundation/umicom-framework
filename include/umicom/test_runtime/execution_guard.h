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
void umi_test_runtime_execution_guard_init(UmiTestRuntimeExecutionGuard *value,const char *id);
UmiStatus umi_test_runtime_execution_guard_validate(const UmiTestRuntimeExecutionGuard *value);
UmiStatus umi_test_runtime_execution_guard_set_category(UmiTestRuntimeExecutionGuard *value,const char *category);
UmiStatus umi_test_runtime_execution_guard_set_detail(UmiTestRuntimeExecutionGuard *value,const char *detail);
UmiStatus umi_test_runtime_execution_guard_set_guard_count(UmiTestRuntimeExecutionGuard *value,uint64_t number);
UmiStatus umi_test_runtime_execution_guard_set_rejected_count(UmiTestRuntimeExecutionGuard *value,uint64_t number);
UmiStatus umi_test_runtime_execution_guard_set_active(UmiTestRuntimeExecutionGuard *value,bool active);
bool umi_test_runtime_execution_guard_same_identity(const UmiTestRuntimeExecutionGuard *left,const UmiTestRuntimeExecutionGuard *right);
#ifdef __cplusplus
}
#endif
#endif
