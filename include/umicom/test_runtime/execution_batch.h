/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_batch.h
 *
 * PURPOSE:
 *   Track one bounded group of tests scheduled together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_BATCH
#define UMICOM_TEST_RUNTIME_EXECUTION_BATCH

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime execution batch data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutionBatch {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t planned_count;
    uint64_t completed_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeExecutionBatch;

/**
 * Initialise test runtime execution batch from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_execution_batch_init(UmiTestRuntimeExecutionBatch *value, const char *id);
/**
 * Check that test runtime execution batch satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_execution_batch_validate(const UmiTestRuntimeExecutionBatch *value);
/**
 * Provide the test runtime execution batch set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_execution_batch_set_name(UmiTestRuntimeExecutionBatch *value, const char *name);
/**
 * Provide the test runtime execution batch set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_batch_set_detail(UmiTestRuntimeExecutionBatch *value, const char *detail);
/**
 * Return the number of records represented by test runtime execution batch set planned
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_batch_set_planned_count(UmiTestRuntimeExecutionBatch *value, uint64_t number);
/**
 * Return the number of records represented by test runtime execution batch set completed
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_batch_set_completed_count(UmiTestRuntimeExecutionBatch *value, uint64_t number);
/**
 * Provide the test runtime execution batch touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_execution_batch_touch(UmiTestRuntimeExecutionBatch *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime execution batch same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_execution_batch_same_identity(const UmiTestRuntimeExecutionBatch *left, const UmiTestRuntimeExecutionBatch *right);

#ifdef __cplusplus
}
#endif
#endif
